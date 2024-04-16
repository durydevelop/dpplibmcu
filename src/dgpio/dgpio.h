#ifndef DGpio_H
#define DGpio_H

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
    #include <derrorcodes.h>
    #include <dgpiochip>

    #define mapValue(x,in_min,in_max,out_min,out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min

    #ifndef LOW
        #define LOW LG_LOW
    #endif
    #ifndef HIGH
        #define HIGH LG_HIGH
    #endif
/*
    #define bitRead(x, n) (((x) >> (n)) & 0x01)
    #define LOWORD(l) ((unsigned short)(l))
    #define HIWORD(l) ((unsigned short)(((unsigned int)(l) >> 16) & 0xFFFF))
    #define LOBYTE(w) ((char)(w&0x00FF))
    #define HIBYTE(w) ((char)((w>>8)&0x00FF))
    #define WORD(msb,lsb) ((msb << 8) | lsb)
    #define DWORD(msw,lsw) ((msw << 16) | lsw)
    #define DWORD_B(msb,next_msb,next_lsb,lsb) (msb << 24) | (next_msb << 16) | (next_lsb << 8) | lsb
*/    
#endif

#include <dutils>

// Enumations
enum DPinMode { INPUT, OUTPUT, INPUT_PULLUP, SOFT_PWM }; //, OUTPUT_PWM };
enum DPinFlags {
    NO_FLAGS=    0,
    ACTIVE_LOW=  4,
    OPEN_DRAIN=  8,
    OPEN_SOURCE= 16,
    PULL_UP=     32,
    PULL_DOWN=   64,
    PULL_NONE=   128
};

DResult initGpio(int gpioDevice, DGpioHandle& handle);
bool isGpioReady(DGpioHandle& handle);
DResult initPin(uint8_t pin, DPinMode mode, DPinFlags flags, DGpioHandle handle);
int readPin(uint8_t pin, DGpioHandle handle);
DResult writePin(uint8_t pin, uint8_t level, DGpioHandle handle);
DResult writeAnalog(unsigned short int pin, unsigned short int value, DGpioHandle handle);
DResult releasePin(uint8_t pin, DGpioHandle handle);
DResult shutdownGpio(DGpioHandle handle);

#endif