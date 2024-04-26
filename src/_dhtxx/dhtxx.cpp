/*
dhtxx.c
2020-11-18
Public Domain

http://abyz.me.uk/lg/lgpio.html

gcc -Wall -o dhtxx dhtxx.c -llgpio

./dhtxx gpio ...
*/
/*
      +-------+-------+
      | DHT11 | DHTXX |
      +-------+-------+
Temp C| 0-50  |-40-125|
      +-------+-------+
RH%   | 20-80 | 0-100 |
      +-------+-------+

         0      1      2      3      4
      +------+------+------+------+------+
DHT11 |check-| 0    | temp |  0   | RH%  |
      |sum   |      |      |      |      |
      +------+------+------+------+------+
DHT21 |check-| temp | temp | RH%  | RH%  |
DHT22 |sum   | LSB  | MSB  | LSB  | MSB  |
DHT33 |      |      |      |      |      |
DHT44 |      |      |      |      |      |
      +------+------+------+------+------+

*/

#include "dhtxx.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include <lgpio.h>


enum DHTReadStatus { DHT_READ_GOOD, DHT_READ_BAD_CHECKSUM, DHT_READ_BAD_DATA, DHT_READ_TIMEOUT };

#define MAX_GPIO 32

void changeCallback(int eventsCount, lgGpioAlert_p events, void *data);

DHTXX *This=nullptr;

DHTXX::DHTXX(int gpioPin, DGpioHandle gpioHandle)
{
    pin=gpioPin;
    model=DHT_AUTO;
    humidity=0;
    temp=0;
    lastResult=DERR_CLASS_NOT_BEGUN;

    //std::cout << "DHTXX() gpioPin=" << gpioPin<< " gpioHandle=" << gpioHandle << std::endl;

    if (gpioHandle < 0) {
        // Try init on first device
        //std::cout << "auto set handle" << std::endl;
        lastResult=initGpio(0,handle);
    }
    else {
        if (isGpioReady(gpioHandle)) {
            handle=gpioHandle;
        }
        else {
            lastResult=DERR_GPIO_NOT_READY;
        }
    }

    This=this;
}

/**
 * @brief Destroy the DHTXX::DHTXX object and free pin use.
 */
DHTXX::~DHTXX()
{
    releasePin(pin,handle);
}

bool DHTXX::begin(DHTModel dhtModel)
{
    model=dhtModel;

    if (handle >= 0) {
        lastResult=initPin(pin,DPinMode::OUTPUT,DPinFlags::NO_FLAGS,handle);
        if (lastResult != DRES_OK) {
            return false;
        }
    }

    // Set callback for pin changes
    lastResult=lgGpioSetAlertsFunc(handle,pin,changeCallback,0);
    if (lastResult != DRES_OK) {
        return false;
    }

    // Wachdog 1 second
    lgGpioSetWatchdog(handle, pin, 1000);
    if (lastResult != DRES_OK) {
        return false;
    }
    
    return true;
}

int DHTXX::decodeReading(uint64_t reading)
{
    std::cout << "decode " << reading << std::endl;
    uint8_t byte[5];
    uint8_t chksum;
    float div;
    float t, h;
    int valid;

    byte[0] = (reading    ) & 255;
    byte[1] = (reading>> 8) & 255;
    byte[2] = (reading>>16) & 255;
    byte[3] = (reading>>24) & 255;
    byte[4] = (reading>>32) & 255;

    chksum = (byte[1] + byte[2] + byte[3] + byte[4]) & 0xFF;

    valid = 0;

    if (chksum == byte[0]) {
        if (model == DHT_11) {
            if ((byte[1] == 0) && (byte[3] == 0)) {
                valid = 1;

                t = byte[2];

                if (t > 60.0) valid = 0;

                h = byte[4];

                if ((h < 10.0) || (h > 90.0)) valid = 0;
            }
        }
        else if (model == DHT_XX) {
            valid = 1;

            h = ((float)((byte[4]<<8) + byte[3]))/10.0;

            if (h > 110.0) valid = 0;

            if (byte[2] & 128) div = -10.0; else div = 10.0;

            t = ((float)(((byte[2]&127)<<8) + byte[1])) / div;

            if ((t < -50.0) || (t > 135.0)) valid = 0;
        }
        else {
            //Auto

            valid = 1;

            // Try DHTXX first

            h = ((float)((byte[4]<<8) + byte[3]))/10.0;

            if (h > 110.0) valid = 0;

            if (byte[2] & 128) div = -10.0; else div = 10.0;

            t = ((float)(((byte[2]&127)<<8) + byte[1])) / div;

            if ((t < -50.0) || (t > 135.0)) valid = 0;

            if (!valid) {
                // If not DHTXX try DHT11

                if ((byte[1] == 0) && (byte[3] == 0)) {
                    valid = 1;

                    t = byte[2];

                    if (t > 60.0) valid = 0;

                    h = byte[4];

                    if ((h < 10.0) || (h > 90.0)) valid = 0;
                }
            }
        }

        if (valid) {
            std::cout << "valid" << std::endl;
            decodeingStatus=DHT_READ_GOOD;
            humidity=h;
            temp=t;
        }
        else {
            std::cout << "bad data" << std::endl;
            decodeingStatus=DHT_READ_BAD_DATA;
        }
    }
    else {
        std::cout << "bad checksum" << std::endl;
        decodeingStatus=DHT_READ_BAD_CHECKSUM;
    }

    return decodeingStatus;
}

int DHTXX::read(void)
{
    lastResult=lgGpioClaimOutput(handle,0,pin,0);
    if (lastResult) {
        return lastResult;
    }

    usleep(15000);

    lastResult=lgGpioClaimAlert(handle,0,LG_RISING_EDGE,pin,-1);
    if (lastResult) {
        return lastResult;
    }

    int count = 0;
    decodeingStatus = DHT_READ_TIMEOUT;

    while ((decodeingStatus == DHT_READ_TIMEOUT) && (++count < 11)) {
        usleep(1000);
    }

    fflush(NULL);

    return decodeingStatus;
}

std::string DHTXX::getLastError(void)
{
    return getErrorCode(lastResult);
}

void changeCallback(int eventsCount, lgGpioAlert_p events, void *data)
{
    uint64_t edge_len, now_tick;
    static int bits = 0;
    static uint64_t reading = 0;
    static uint64_t last_tick = 0;

    for (int ixE=0; ixE<eventsCount; ixE++) {
        if (events[ixE].report.level != LG_TIMEOUT) {
            now_tick = events[ixE].report.timestamp;
            edge_len = now_tick - last_tick;
            last_tick = now_tick;
            if (edge_len > 1e6) {
                // a millisecond
                reading = 0;
                bits = 0;
            }
            else {
                reading <<= 1;
                if (edge_len > 1e5) reading |= 1; // longer than 100 micros
                ++bits;
            }
        }
        else {
            This->decodeReading(reading);
            reading = 0;
            bits = 0;
        }
    }
}