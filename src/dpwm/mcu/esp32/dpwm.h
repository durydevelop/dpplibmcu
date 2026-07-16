#ifndef DSOFTPWM_H
#define DSOFTPWM_H

#ifdef ARDUINO
    #ifdef PLATFORMIO
        #include "../dgpio/dgpio.h"
    #endif
#else
    #include <dgpio>
#endif

class DPwmOut {
    public:
        DPwmOut(uint8_t GpioPin);
        ~DPwmOut();
        void release(void);
        bool begin(uint16_t FreqHz = 0, uint8_t DutyCyclePerc = 50, bool Activate = false);
        void set(uint16_t FreqHz, uint8_t DutyCyclePerc, bool Activate);
        //void SetHz(uint16_t FreqHz);
        void setHz(uint16_t FreqHz);
        void setDutyPerc(uint8_t DutyCyclePerc);
        void setMicros(uint16_t Us);
        void on(bool PwmOn = true);
        void high(void);
        void low(void);
        void printInfo(void);
        bool isReady(void);
        bool isActive(void);
        uint16_t getPeriodUs(void);
        uint8_t getPin(void);
        uint8_t getDutyPerc(void);

        uint8_t Pin;
        uint8_t BitMask=0;
        volatile uint8_t *OUTPORT;
        bool Active=false;       //! Pwm active (default off).
        uint32_t TickCounter=0;  //! ISR call counter
        uint32_t TicksON=0;      //! Number of ticks for HIGH time.
        uint32_t TicksTOT=0;     //! Numebr of ticks of total period.
        uint8_t dutyPerc;

};

#endif