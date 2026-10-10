#include "di2c.h"


// You may change the communication speed here
//#define i2cDelay() delayMicroseconds(3) //  68kHz@80Mhz or  83kHz@160Mhz
//#define i2cDelay() delayMicroseconds(2) //  85kHz@80Mhz or 110kHz@160Mhz
//#define i2cDelay() delayMicroseconds(1) // 110kHz@80Mhz or 170kHz@160Mhz
//#define i2cDelay() delayMicroseconds(0) // 150kHz@80Mhz or 300kHz@160Mhz
#define i2cDelay()                      // 250kHz@80Mhz or 500kHz@160Mhz

// ------------------------------------- Public methods -------------------------------------
DI2CMaster::DI2CMaster(uint8_t sda_pin, uint8_t scl_pin)
{
    sda = sda_pin;
    scl = scl_pin;
}

void DI2CMaster::begin()
{
    #if defined OUTPUT_OPEN_DRAIN
        pinMode(sda, OUTPUT_OPEN_DRAIN);
        digitalWrite(sda, HIGH);
        pinMode(scl, OUTPUT_OPEN_DRAIN);
        digitalWrite(scl, HIGH);
    #else
        pinMode(sda, INPUT);
        pinMode(scl, INPUT);
    #endif
}

bool DI2CMaster::startWrite(uint8_t slaveAddr)
{
    i2cStart();
    return i2cSend(slaveAddr << 1);
}

bool DI2CMaster::startRead(uint8_t slaveAddr)
{
    i2cStart();
    return i2cSend((slaveAddr << 1) | 1);
}

bool DI2CMaster::write(uint8_t data)
{
    return i2cSend(data);
}

bool DI2CMaster::read(uint8_t &data, bool ack)
{
    data = i2cRecv(ack);
    return true;
}

void DI2CMaster::stop()
{
    i2cStop();
}

// ------------------------------------- Private methods -------------------------------------
void DI2CMaster::i2cStart()
{
    while (!digitalRead(sda) || !digitalRead(scl));
    sdaLow();
    i2cDelay();
    sclLow();
    i2cDelay();
}

void DI2CMaster::i2cStop()
{
    sclHigh();
    i2cDelay();
    sdaHigh();
    i2cDelay();
    while (!digitalRead(sda) || !digitalRead(scl));
}

bool DI2CMaster::i2cSend(uint8_t byte)
{
    for (int_fast8_t bit = 7; bit >= 0; bit--)
    {
        if (byte & 128) {
            sdaHigh();
        }
        else {
            sdaLow();
        }
        i2cDelay();
        // Clock ON
        sclHigh();        
        i2cDelay();
        // Clock stretching: wait for slave to release clock line (wait for scl_pin to go HIGH)
        while (!digitalRead(scl)); 
        // Clock OFF
        sclLow();
        i2cDelay();
        byte = byte << 1;
    }
    // Receive ACK
    // Libera SDA (diventa input, lo slave la può tirare giù
    sdaHigh();
    // Clock ON
    sclHigh();    
    i2cDelay();
    // Clock stretching: wait for slave to release clock line (wait for scl to go HIGH)
    while (!digitalRead(scl));
    // ACK = SDA tirata a LOW dallo slave
    bool ack = digitalRead(sda) == 0;    
    sclLow();
    i2cDelay();
    return ack;
}

uint8_t DI2CMaster::i2cRecv(bool send_ack)
{
    uint8_t byte = 0;

    // Rilascia SDA: da qui in poi è lo slave a pilotarla
    sdaHigh();

    for (int_fast8_t bit = 7; bit >= 0; bit--) {
        byte = byte << 1;
        sclHigh();
        i2cDelay();
        while (!digitalRead(scl));   // clock stretching

        if (digitalRead(sda)) {
            byte |= 1;
        }
        sclLow();
        i2cDelay();
    }

    // Il master genera ACK (continua a leggere) o NACK (ultimo byte)
    if (send_ack) {
        sdaLow();
    }
    else {
        sdaHigh();
    }
    i2cDelay();
    sclHigh();
    i2cDelay();
    while (!digitalRead(scl));
    sclLow();
    i2cDelay();

    return byte;
}

void DI2CMaster::sdaHigh()
{
    #if !defined OUTPUT_OPEN_DRAIN
        pinMode(sda, INPUT_PULLUP);
    #endif
    digitalWrite(sda, HIGH);
}

void DI2CMaster::sdaLow()
{
    digitalWrite(sda, LOW);
    #if !defined OUTPUT_OPEN_DRAIN
        pinMode(sda, OUTPUT);
    #endif
}

void DI2CMaster::sclHigh()
{
    #if !defined OUTPUT_OPEN_DRAIN
        pinMode(scl, INPUT_PULLUP);
    #endif
    digitalWrite(scl, HIGH);
}

void DI2CMaster::sclLow()
{
    digitalWrite(scl, LOW);
    #if !defined OUTPUT_OPEN_DRAIN
        pinMode(scl, OUTPUT);
    #endif
}

// ------------------------------------- Convenient helpers ----------------------------------
bool DI2CMaster::sendByte(uint8_t slaveAddr, uint8_t data) {
    i2cStart();
    i2cSend(slaveAddr << 1);  // address + write
    i2cSend(data);
};

bool DI2CMaster::sendWord(uint8_t slaveAddr, uint16_t data) {
    i2cStart();
    i2cSend(slaveAddr << 1);  // address + write
    i2cSend((data >> 8) & 0xFF); // MSB
    i2cSend(data & 0xFF);        // LSB
};

bool DI2CMaster::sendDword(uint8_t slaveAddr, uint32_t data) {
    i2cStart();
    i2cSend(slaveAddr << 1);  // address + write
    i2cSend((data >> 24) & 0xFF); // MSB
    i2cSend((data >> 16) & 0xFF);
    i2cSend((data >> 8) & 0xFF);
    i2cSend(data & 0xFF);        // LSB
};

bool DI2CMaster::sendBuf(uint8_t slaveAddr, const uint8_t *data, size_t len) {
    i2cStart();
    i2cSend(slaveAddr << 1);  // address + write
    for (size_t i = 0; i < len; i++) {
        i2cSend(data[i]);
    }
    i2cStop();
    return true;
};