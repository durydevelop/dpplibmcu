/*
dhtxx.c
2020-11-18
Public Domain

http://abyz.me.uk/lg/lgpio.html

gcc -Wall -o dhtxx dhtxx.c -llgpio

./dhtxx gpio ...
*/
/*
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include <lgpio.h>
*/

#ifndef DHTXX_H
#define DHTXX_H

#include <dgpio>

class DHTXX
{
    public:
        enum DHTModel { DHT_AUTO, DHT_11, DHT_XX };

        DHTXX(int pin, DGpioHandle gpioHandle = -1);
        ~DHTXX();

        bool begin(DHTModel dhtModel);
        int read(void);
        int decodeReading(uint64_t reading);

        std::string getLastError(void);

        float humidity;
        float temp;
        int decodeingStatus;

    private:
        

        int pin;
        DHTModel model;
        DGpioHandle handle;
        DResult lastResult;

        
        

};

#endif