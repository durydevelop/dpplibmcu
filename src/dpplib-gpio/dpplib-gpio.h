// Platform check
#ifdef ARDUINO
    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "wiring.h"
        #include "WProgram.h"
    #endif
#else
    // Raspberry Pi
    #include <cstdlib>
    // include lib which want to use (if both are included, pigpio is used)
    #include <pigpio.h>
    //#include <wiringPi.h>

    #define map(x,in_min,in_max,out_min,out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min
    #ifndef LOW
        #define LOW PI_LOW
    #endif
    #ifndef HIGH
        #define HIGH PI_HIGH
    #endif
#endif

enum DPinMode { INPUT_MODE, OUTPUT_MODE, INPUT_PULLUP_MODE , OUTPUT_PWM_MODE };

bool InitPin(uint8_t Pin, uint8_t Mode);
uint8_t ReadPin(uint8_t Pin);
void WritePin(uint8_t Pin, uint8_t Level);
//void PwmWrite(unsigned short int Pin, unsigned short int Value);
void SetPullUp(short Pin);
#ifdef PIGPIO_VERSION
    void delay(unsigned long ms);
    unsigned long millis(void);
    unsigned long micros(void);
//	void SetPullDown(short Pin);
//	void SetFloating(short Pin);
#endif
void ShutdownGpio(void);

unsigned long Now(void);
