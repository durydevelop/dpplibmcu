#ifndef DGpio_H
#define DGpio_h

// Platform check
#ifdef ARDUINO
    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "wiring.h"
        #include "WProgram.h"
    #endif
#else
    // Not arduino (rpi or linux sbc)
    #include <lgpio.h>

    #define map(x,in_min,in_max,out_min,out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min
    #ifndef LOW
        #define LOW LG_LOW
    #endif
    #ifndef HIGH
        #define HIGH LG_HIGH
    #endif

    static int chip=-1;
    
    bool initGpio(int gpioDevice = 0);
    bool shutdownGpio(int gpioDevice = 0);
#endif

#include <utils.h>

// Enumations
enum DPinMode { INPUT, OUTPUT, INPUT_PULLUP }; //, OUTPUT_PWM };
enum DPinFlags {
    NO_FLAGS=    0,
    ACTIVE_LOW=  4,
    OPEN_DRAIN=  8,
    OPEN_SOURCE= 16,
    PULL_UP=     32,
    PULL_DOWN=   64,
    PULL_NONE=   128
};

bool initPin(uint8_t pin, DPinMode mode, DPinFlags flags = NO_FLAGS);

uint8_t readPin(uint8_t pin);
bool writePin(uint8_t pin, uint8_t level);
bool writeAnalog(unsigned short int pin, unsigned short int value);

#endif