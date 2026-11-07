#ifndef DSOFTPWM_H
#define DSOFTPWM_H

#ifdef ARDUINO_ARCH_AVR
#ifdef PLATFORMIO
    #include "../dgpio/dgpio.h"
#else
    #include <dgpio>
#endif

class DPwmOut {
    public:
        DPwmOut(uint8_t gpioPin);
        ~DPwmOut();
        void release(void);
        bool begin(uint16_t freqHz = 0, uint8_t dutyCyclePerc = 50, bool activate = false);
        void set(uint16_t freqHz, uint8_t dutyCyclePerc, bool activate);
        //void SetHz(uint16_t FreqHz);
        void setHz(uint16_t freqHz);
        void setDutyPerc(uint8_t dutyCyclePerc);
        void setMicros(uint16_t us);
        void on(bool pwmOn = true);
        void high(void);
        void low(void);
        void printInfo(void);
        bool isReady(void);
        bool isActive(void);
        uint16_t getPeriodUs(void);
        uint8_t getPin(void);
        uint8_t getDutyPerc(void);

        uint8_t pin;
        uint8_t bitMask=0;
        volatile uint8_t *OUTPORT;
        bool active=false;       //! Pwm active (default off).
        uint32_t tickCounter=0;  //! ISR call counter
        uint32_t ticksON=0;      //! Number of ticks for HIGH time.
        uint32_t ticksTOT=0;     //! Numebr of ticks of total period.
        uint8_t currDutyPerc;    //! Current duty cycle percentage

};

#endif
#endif