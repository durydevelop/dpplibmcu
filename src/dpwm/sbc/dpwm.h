#ifndef DPwm_H
#define DPwm_H

#include <dgpio>

class DPwmOut {
    public:
        DPwmOut(int gpioPin, DGpioHandle gpioHandle = -1);
        ~DPwmOut();
        
        void release(void);
        bool begin(float frequecyHz = 0.0, float dutyCyclePerc = 50.0, bool activate = false);
        bool set(float frequecyHz, float dutyCyclePerc, bool activate);
        bool setHz(float frequecyHz);
        bool setDutyPerc(float dutyCyclePerc);
        bool setMicros(uint16_t us);
        bool on(bool pwmOn = true);
        bool high(void);
        bool low(void);
        bool isReady(void);
        bool isActive(void);
        uint16_t getPeriodUs(void);
        uint8_t getPin(void);
        uint8_t getDutyPerc(void);
        std::string getLastError(void);

        int pin;
        bool active;    //! Pwm active (default off).
        float freqHz;
        float dutyPerc;

        DGpioHandle handle;
        DResult lastResult;
};

#endif