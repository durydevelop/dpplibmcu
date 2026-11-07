#ifndef DSOFTPWM_H
#define DSOFTPWM_H

#ifdef ARDUINO_ARCH_ESP32

#include <dgpio>

class DPwmOut {
    public:
        DPwmOut(uint8_t gpioPin, bool invertOutput = false);
        ~DPwmOut();
        void release(void);
        uint8_t begin(uint16_t freqHz = 0, uint8_t dutyCyclePerc = 50, bool activate = false);
        void set(uint16_t freqHz, uint8_t dutyCyclePerc, bool activate);
        void setHz(uint16_t freqHz);
        void setDutyPerc(uint8_t dutyCyclePerc);
        void setMicros(uint16_t us);
        void on(bool pwmOn = true);
        void high(void);
        void low(void);
        bool isReady(void);
        bool isActive(void);
        uint16_t getPeriodUs(void);
        uint8_t getPin(void);
        uint8_t getDutyPerc(void);
        void setInvertedOut(bool invertOutput);

        String getInfo(void);

        uint8_t pin;              //! Pin used
        bool active=false;        //! Pwm out active
        uint8_t currDutyPerc=0;   //! Current duty cycle percentage
        uint8_t channel=255;      //! ESP32 LEDC channel
        uint8_t pwmResolution=10; //! ESP32 PWM resolution bits
        uint16_t pwmMaxDuty=1023; //! Derived from pwmResolution
        uint32_t currfreqHz=0;    //! Current PWM frequency
        bool invertedOut=false;   //! Invert PWM output for active-low loads

};

#endif
#endif