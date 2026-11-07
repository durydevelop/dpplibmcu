#ifdef ARDUINO_ARCH_ESP32

#include "dpwm.h"

/**
 * N.B.
 * To keep trace of all LEDC channels used for DPWM, a static list of DPWM instances is created and the LEDC channel is assigned to each instance.
 * TODO: Set / get PWM resolution (bits) and max duty cycle (value) for each instance.
 */

#define MAX_PWM_CHANNELS 5
#define PWM_RESOLUTION_BITS 10

// Pwm channels list
DPwmOut *PwmList[MAX_PWM_CHANNELS];
bool Initialized=false;

// ********************************************************************************
// ****************************** DPwmOut class ***********************************
// ********************************************************************************

static inline uint32_t dutyFromPercent(uint8_t dutyPerc, uint16_t maxDuty)
{
    return ((uint32_t)dutyPerc * maxDuty) / 100;
}

DPwmOut::DPwmOut(uint8_t gpioPin, bool invertOutput) {
    pin = gpioPin;
    active = false;
    currDutyPerc = 0;
    channel = 255;
    pwmResolution = PWM_RESOLUTION_BITS;
    pwmMaxDuty = (1 << PWM_RESOLUTION_BITS) - 1;
    currfreqHz = 0;
    invertedOut = invertOutput;
}

DPwmOut::~DPwmOut()
{
    release();
}

void DPwmOut::release(void)
{
    for (uint8_t ixP = 0; ixP < MAX_PWM_CHANNELS; ixP++) {
        if (PwmList[ixP] == this) {
            PwmList[ixP] = nullptr;
            break;
        }
    }
}

uint8_t DPwmOut::begin(uint16_t freqHz, uint8_t dutyCyclePerc, bool startPwm)
{
    if (!Initialized) {
        for (uint8_t ixC = 0; ixC < MAX_PWM_CHANNELS; ixC++) {
            PwmList[ixC] = nullptr;
        }
        Initialized = true;
    }

    bool found = false;
    for (uint8_t ixC = 0; ixC < MAX_PWM_CHANNELS; ixC++) {
        if (PwmList[ixC] == nullptr) {
            PwmList[ixC] = this;
            channel = ixC;
            found = true;
            break;
        }
    }
    if (!found) {
        return 255;
    }

    pinMode(pin, OUTPUT);
    ledcAttachChannel(pin, freqHz, pwmResolution, channel);
    set(freqHz, dutyCyclePerc, startPwm);
    return channel;
}

void DPwmOut::set(uint16_t freqHz, uint8_t dutyCyclePerc, bool startPwm)
{
    if (channel == 255) {
        return;
    }

    if (freqHz == 0) {
        low();
        return;
    }

    if (freqHz != currfreqHz) {
        currfreqHz = freqHz;
        ledcChangeFrequency(pin, freqHz, pwmResolution);
    }

    setDutyPerc(dutyCyclePerc);
    active = startPwm;
    if (!active) {
        low();
    }
}

void DPwmOut::setHz(uint16_t freqHz)
{
   set(freqHz, currDutyPerc, true);
}

void DPwmOut::setDutyPerc(uint8_t dutyCyclePerc)
{
    if (channel == 255) {
        return;
    }

    if (currfreqHz == 0) {
        return;
    }

    currDutyPerc = dutyCyclePerc;
    if (currDutyPerc == 0) {
       low();
       return;
    }
    else if (currDutyPerc >= 100) {
        currDutyPerc = 100;
        high();
        return;
    }

    uint32_t duty = dutyFromPercent(currDutyPerc, pwmMaxDuty);
    if (invertedOut) {
        duty = pwmMaxDuty - duty;
    }
    ledcWrite(pin, duty);
    active = true;
}

void DPwmOut::setMicros(uint16_t us)
{
    if (channel == 255) {
        return;
    }

    if (currfreqHz == 0) {
        return;
    }

    uint32_t periodUs = getPeriodUs();
    if (us == 0) {
        currDutyPerc = 0;
        low();
    }
    else if (us >= periodUs) {
        currDutyPerc = 100;
        high();
    }
    else {
        uint32_t duty = ((uint32_t)us * pwmMaxDuty) / periodUs;
        currDutyPerc = (uint8_t)((100U * duty) / pwmMaxDuty);
        if (invertedOut) {
            duty = pwmMaxDuty - duty;
        }
        ledcWrite(pin, duty);
        active = true;
    }
}

void DPwmOut::on(bool pwmOn)
{
    if (channel == 255) {
        return;
    }

    if (pwmOn) {
        active = true;
    }
    else {
        low();
    }
}

void DPwmOut::high(void)
{
    if (channel == 255) {
        return;
    }
    active = false;
    ledcWrite(pin, invertedOut ? 0 : pwmMaxDuty);
}

void DPwmOut::low(void)
{
    if (channel == 255) {
        return;
    }
    active = false;
    ledcWrite(pin, invertedOut ? pwmMaxDuty : 0);
}

bool DPwmOut::isReady(void)
{
    return (channel != 255);
}

bool DPwmOut::isActive(void) {
    return (active);
}

uint16_t DPwmOut::getPeriodUs(void)
{
    return (currfreqHz == 0 ? 0 : (uint16_t)(1000000UL / currfreqHz));
}

uint8_t DPwmOut::getPin(void)
{
    return (pin);
}

uint8_t DPwmOut::getDutyPerc(void)
{
    return (currDutyPerc);
}

void DPwmOut::setInvertedOut(bool invertOutput)
{
    invertedOut = invertOutput;
}

String DPwmOut::getInfo(void)
{
    String info="\
        GpioPin    " + String(pin) + "\n\
        Channel    " + String(channel) + "\n\
        Resolution " + String(pwmResolution) + "\n\
        Max duty   " + String(pwmMaxDuty) + "\n\
        Freq Hz    " + String(currfreqHz) + "\n\
        Duty %     " + String(currDutyPerc);

    return info;
}

#endif
