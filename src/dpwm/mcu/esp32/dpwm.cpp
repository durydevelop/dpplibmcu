#ifdef ARDUINO_ARCH_ESP32

#include <Arduino.h>
#include "dpwm.h"

#define MAX_PWM_COUNT 5
#define PWM_RESOLUTION_BITS 10

DPwmOut *PwmList[MAX_PWM_COUNT];
bool Initialized=false;

#if defined(HAVE_HWSERIAL0)
    #define Debug(s) Serial.println(s)
#else
    #define Debug(s)
#endif

// ********************************************************************************
// ****************************** DPwmOut class ***********************************
// ********************************************************************************

static inline uint32_t dutyFromPercent(uint8_t dutyPerc, uint16_t maxDuty)
{
    return ((uint32_t)dutyPerc * maxDuty) / 100;
}

DPwmOut::DPwmOut(uint8_t GpioPin) {
    Pin = GpioPin;
    BitMask = 0;
    OUTPORT = nullptr;
    Active = false;
    TickCounter = 0;
    TicksON = 0;
    TicksTOT = 0;
    dutyPerc = 0;
    Channel = 255;
    pwmResolution = PWM_RESOLUTION_BITS;
    pwmMaxDuty = (1 << PWM_RESOLUTION_BITS) - 1;
    freqHz = 0;
}

DPwmOut::~DPwmOut()
{
    release();
}

void DPwmOut::release(void) {
    for (uint8_t ixP = 0; ixP < MAX_PWM_COUNT; ixP++) {
        if (PwmList[ixP] == this) {
            PwmList[ixP] = nullptr;
            break;
        }
    }
}

bool DPwmOut::begin(uint16_t FreqHz, uint8_t DutyCyclePerc, bool Activate) {
    if (!Initialized) {
        for (uint8_t ixC = 0; ixC < MAX_PWM_COUNT; ixC++) {
            PwmList[ixC] = nullptr;
        }
        Initialized = true;
    }

    bool Found = false;
    for (uint8_t ixC = 0; ixC < MAX_PWM_COUNT; ixC++) {
        if (PwmList[ixC] == nullptr) {
            PwmList[ixC] = this;
            Channel = ixC;
            Found = true;
            break;
        }
    }
    if (!Found) {
        Debug("No free slot");
        return false;
    }

    pinMode(Pin, OUTPUT);
    pwmResolution = PWM_RESOLUTION_BITS;
    pwmMaxDuty = (1 << pwmResolution) - 1;
    freqHz = (FreqHz == 0 ? 1 : FreqHz);
    ledcAttachChannel(Pin, freqHz, pwmResolution, Channel);
    set(FreqHz, DutyCyclePerc, Activate);
    return true;
}

void DPwmOut::set(uint16_t freqHz, uint8_t dutyCyclePerc, bool activate) {
    if (Channel == 255) {
        return;
    }

    if (freqHz == 0) {
        low();
        return;
    }

    if (freqHz != this->freqHz) {
        this->freqHz = freqHz;
        ledcChangeFrequency(Pin, freqHz, pwmResolution);
    }

    TicksTOT = pwmMaxDuty;
    TicksON = dutyFromPercent(dutyCyclePerc, pwmMaxDuty);
    TickCounter = 0;
    setDutyPerc(dutyCyclePerc);
    Active = activate;
    if (!activate) {
        low();
    }
}

void DPwmOut::setHz(uint16_t FreqHz) {
   set(FreqHz, dutyPerc, true);
}

void DPwmOut::setDutyPerc(uint8_t dutyCyclePerc)
{
    if (Channel == 255) {
        return;
    }

    if (freqHz == 0) {
        return;
    }

    dutyPerc = dutyCyclePerc;
    if (dutyPerc == 0) {
       low();
       return;
    }
    else if (dutyPerc >= 100) {
        dutyPerc = 100;
        high();
        return;
    }

    uint32_t duty = dutyFromPercent(dutyPerc, pwmMaxDuty);
    TicksON = duty;
    TicksTOT = pwmMaxDuty;
    ledcWrite(Pin, duty);
    Active = true;
}

void DPwmOut::setMicros(uint16_t Us)
{
    if (Channel == 255) {
        return;
    }

    if (freqHz == 0) {
        return;
    }

    uint32_t periodUs = getPeriodUs();
    if (Us == 0) {
        dutyPerc = 0;
        low();
    }
    else if (Us >= periodUs) {
        dutyPerc = 100;
        high();
    }
    else {
        uint32_t duty = ((uint32_t)Us * pwmMaxDuty) / periodUs;
        TicksON = duty;
        TicksTOT = pwmMaxDuty;
        dutyPerc = (uint8_t)((100U * duty) / pwmMaxDuty);
        ledcWrite(Pin, duty);
        Active = true;
    }
}

void DPwmOut::on(bool PwmOn) {
    if (Channel == 255) {
        return;
    }

    if (PwmOn) {
        Active = true;
    }
    else {
        low();
    }
}

void DPwmOut::high(void) {
    if (Channel == 255) {
        return;
    }
    Active = false;
    ledcWrite(Pin, pwmMaxDuty);
}

void DPwmOut::low(void) {
    if (Channel == 255) {
        return;
    }
    Active = false;
    ledcWrite(Pin, 0);
}

bool DPwmOut::isReady(void)
{
    return (Channel != 255);
}

bool DPwmOut::isActive(void) {
    return (Active);
}

uint16_t DPwmOut::getPeriodUs(void)
{
    return (freqHz == 0 ? 0 : (uint16_t)(1000000UL / freqHz));
}

uint8_t DPwmOut::getPin(void)
{
    return (Pin);
}

uint8_t DPwmOut::getDutyPerc(void) {
    return (dutyPerc);
}

void DPwmOut::printInfo(void) {
    #if defined(HAVE_HWSERIAL0)
        noInterrupts();
        Serial.print("GpioPin "); Serial.println(Pin);
        Serial.print("Channel "); Serial.println(Channel);
        Serial.print("Resolution "); Serial.println(pwmResolution);
        Serial.print("Max duty "); Serial.println(pwmMaxDuty);
        Serial.print("Freq Hz "); Serial.println(freqHz);
        Serial.print("Duty % "); Serial.println(dutyPerc);
        interrupts();
    #endif
}

#endif
