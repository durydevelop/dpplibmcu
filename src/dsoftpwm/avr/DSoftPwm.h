#ifndef DSOFTPWM_H
#define DSOFTPWM_H

struct DPwm{
    int Pin=LED_BUILTIN;    //! GPIO pin number (default PB1).
    bool Active=true;       //! Pwm active      (default on).
    uint32_t TickCounter=0; //! ISR call counter
    uint32_t TicksON=0;     //! Number of ticks for HIGH time.
    uint32_t TicksTOT=0;    //! Numebr of ticks of total period.
};

class DPwmOut {
    public:
        DPwmOut(uint8_t GpioPin);
        ~DPwmOut();
        void begin(uint16_t FreqHz = 0, uint8_t DutyCyclePerc = 0, bool Start = false);
        bool IsReady(void);
        bool SetFreq(uint16_t FreqHz, uint8_t DutyCyclePerc);
        bool ChangeDutyCycle(uint8_t DutyCyclePerc);
        void SetActive(bool ActiveOut);
        void On(void);
        void Off(void);

    private:
        //void SetupTimer(void);
        DPwm *dPwm;
        const uint8_t Pin;      //! GPIO pin number (default PB1).

    private:
        bool Ready;
};

#endif