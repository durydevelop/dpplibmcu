#include <Arduino.h>
#include "DSoftPwm.h"
#include "DSoftPwmTimer.h"
#include <vector>

#define MAX_PWM_COUNT 10
#define US_PER_TICK (TIMER_FREQ/TIMER_FREQ)

static std::vector<DPwm*> PwmList; //! List of DPwmOut objects to handle (one for Pin).

// ********************************************************************************
// ********************************* ISR routine **********************************
// ********************************************************************************

ISR(TIMER0_COMPA_vect)
{
    for (uint8_t ixPwm=0; ixPwm<PwmList.size(); ixPwm++) {
        DPwm *Pwm=PwmList.at(ixPwm);
        if (Pwm->Active) {
            Pwm->TickCounter++;
            if (Pwm->TickCounter >= Pwm->TicksTOT) {
                // Pwm period end
                digitalWrite(Pwm->Pin,HIGH);
                Pwm->TickCounter=0;
            }
            else if (Pwm->TickCounter >= Pwm->TicksON) {
                // Pwm HIGH level time reached
                digitalWrite(Pwm->Pin,LOW);
            }
        }
    }
}

// ********************************************************************************
// ****************************** DPwmOut class ***********************************
// ********************************************************************************
DPwmOut::DPwmOut(uint8_t GpioPin) :
Pin(GpioPin)
{
    dPwm=new DPwm();
    dPwm->Pin=Pin;
    Ready=true;
    // Check if Pin is in use
    for (uint8_t ixP=0; ixP<PwmList.size(); ixP++) {
        if (PwmList[ixP]->Pin == Pin) {
            // Already in use
            Ready=false;
            dPwm->Active=false;
            break;
        }
    }

    if (Ready) {
        pinMode(Pin,OUTPUT);
        PwmList.emplace_back(dPwm);
    }
}

DPwmOut::~DPwmOut()
{
    delete dPwm;
    
    if (Ready) {
        // Erase Pwm from PwmList
        for (uint8_t ixP=0; ixP<PwmList.size(); ixP++) {
            if (PwmList[ixP]->Pin == Pin) {
                PwmList.erase(ixP);
                break;
            }
        }
    }
}

void DPwmOut::begin(uint16_t FreqHz, uint8_t DutyCyclePerc, bool Start)
{
    SetupTimer();
    if (SetFreq(FreqHz,DutyCyclePerc)) {
        dPwm->Active=Start;
    }
}

bool DPwmOut::IsReady(void)
{
    return(Ready);
}

/**
 * @brief 
 * Pwm freq must from 1 to 1/2 Timer freq
 * Duty-cycle must be from 1 to 99
 * 
 * @param FreqHz 
 * @param DutyCyclePerc 
 * @return true 
 * @return false 
 */
bool DPwmOut::SetFreq(uint16_t FreqHz, uint8_t DutyCyclePerc)
{
    if (!Ready) {
        return false;
    }

    // Pwm freq must from 1 to 1/2 Timer freq
    if (FreqHz == 0) {
        // disable pin
        dPwm->Active=false;
        return false;
    }
    else if (FreqHz > (TIMER_FREQ/2)) {
        // set to max
        FreqHz=TIMER_FREQ/2;
    }

    // Duty-cycle must be from 1 to 99
    if (DutyCyclePerc == 0) {
        // disable (always off)
       dPwm->Active=false;
    }
    else if (DutyCyclePerc >= 100) {
        // disable
        dPwm->Active=false;
        // set always on
        digitalWrite(Pin,HIGH);
        
    }
    else {
        DutyCyclePerc=DutyCyclePerc;
    }

    dPwm->TicksTOT=(TIMER_FREQ/FreqHz);
    //if (dPwm->TickCounter > dPwm->TicksTOT) {
    //    dPwm->TickCounter=dPwm->TicksTOT-1;
    //}
    ChangeDutyCycle(DutyCyclePerc);

    return true;
}

bool DPwmOut::ChangeDutyCycle(uint8_t DutyCyclePerc)
{
    if (dPwm->TicksTOT == 0) {
        return false;
    }
    dPwm->TicksON=(dPwm->TicksTOT*DutyCyclePerc)/100;
    return true;
}

void DPwmOut::SetActive(bool ActiveOut)
{
    if (Ready) {
        dPwm->Active=ActiveOut;
        //if (!ActiveOut) {
            // Wait al least 2 ticks
            //delayMicroseconds(US_PER_TICK*2);
            //digitalWrite(Pin,LOW);
        //}
    }
}

void DPwmOut::On(void)
{
    if (Ready) {
        dPwm->Active=true;
    }
}

void DPwmOut::Off(void)
{
    if (Ready) {
        dPwm->Active=false;
        // Wait al least 2 ticks
        //delayMicroseconds(US_PER_TICK*2);
        //digitalWrite(Pin,LOW);
    }
}
