#ifdef ARDUINO_ARCH_AVR

#include <Arduino.h>
#include "dpwm.h"
#include "DSoftPwmTimer.h"

/**
 * N.B.
 * There is only one ISR to handle many instatnces of DPWM, so the workaround is
 * to create a static list of DPWM instances and call the ISR for each instance.
 * 
*/

#define MAX_PWM_COUNT 5

// List of pwm channels
DPwmOut *PwmList[MAX_PWM_COUNT];
bool Initialized=false;

// ********************************************************************************
// ********************************* ISR routine **********************************
// ********************************************************************************
//size_t TickCounter=0;
#if defined(__AVR_ATtiny85__)
    #ifdef DPWM_USE_TIMER1
        ISR(TIMER1_OVF_vect)
    #else
        ISR(TIMER0_COMPA_vect)
    #endif
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    #ifdef DPWM_USE_TIMER2
        ISR(TIMER2_COMPA_vect)
    #else
        ISR(TIMER1_COMPA_vect)
    #endif
#else
    #error "DPwm have no support for this platform"
#endif
{
    for (uint8_t ixC=0; ixC<MAX_PWM_COUNT; ixC++) {
        if (PwmList[ixC] ) {
            if (PwmList[ixC]->active) {
                PwmList[ixC]->tickCounter++;
                if (PwmList[ixC]->tickCounter >= PwmList[ixC]->ticksTOT) {
                    // Pwm period end, set to HIGH
                    *PwmList[ixC]->OUTPORT|=PwmList[ixC]->bitMask;
                    PwmList[ixC]->tickCounter=0;
                }
                else if (PwmList[ixC]->tickCounter >= PwmList[ixC]->ticksON) {
                    // Pwm HIGH level time reached, change to LOW
                    *PwmList[ixC]->OUTPORT&=~PwmList[ixC]->bitMask;
                }
            }
        }
    }
}

// ********************************************************************************
// ****************************** DPwmOut class ***********************************
// ********************************************************************************

/**
 * @brief Construct a new DPwmOut::DPwmOut object.
 * N.B. Timer should have not been initialized in costructor (may be it doe not work).
 * Timer will be initialized in Begin().
 */
DPwmOut::DPwmOut(uint8_t gpioPin) {
    // Set defaults
    pin=gpioPin;
    bitMask=0;
    OUTPORT=nullptr;
    active=false;
    tickCounter=0;
    ticksON=0;
    ticksTOT=0;
}

/**
 * @brief Destroy the DPwmOut::DPwmOut object and free slot on PwmList[].
 */
DPwmOut::~DPwmOut()
{
    release();
}

/**
 * @brief Delete this instance pointer from PwmList.
 * N.B. This function only null the slot in PwmList, does not free memory.
 * To do it you must call class destructor.
 * 
 */
void DPwmOut::release(void) {
    for (uint8_t ixP=0; ixP<MAX_PWM_COUNT; ixP++) {
        if (PwmList[ixP] == this) {
            // It's me, null slot
            PwmList[ixP]=nullptr;
            break;
        }
    }
}

/**
 * @brief Init all stuffs for starting pwm on GpioPin.
 * MUST be called before using the class.
 * 
 * @param GpioPin 
 * @param FreqHz 
 * @param DutyCyclePerc 
 * @param Activate 
 */
bool DPwmOut::begin(uint16_t freqHz, uint8_t dutyCyclePerc, bool activate) {
    // Init globals
    if (!Initialized) {
        // Execute only on first DPwmOut creation.
        // Init PwmList
        for (uint8_t ixC=0; ixC<MAX_PWM_COUNT; ixC++) {
            PwmList[ixC]=nullptr;
        }
        // Setup timer
        SetupTimer();
        Initialized=true;
    }

    // Find free slot
    bool Found=false;
    for (uint8_t ixC=0; ixC<MAX_PWM_COUNT; ixC++) {
        if (PwmList[ixC] == nullptr) {
            PwmList[ixC]=this;
            Found=true;
            break;
        }
    }
    if (!Found) {
        return false;
    }

    // Get port register for pin
    uint8_t Port=digitalPinToPort(pin);
    if (Port == NOT_A_PIN) {
        return false;
    }

    // Set all stuff
    pinMode(pin,OUTPUT);
    bitMask=digitalPinToBitMask(pin);
    OUTPORT=portOutputRegister(Port);
    set(freqHz,dutyCyclePerc,activate);
    return true;
}

/**
 * @brief Set pwm frequency and duty-cycle.
 * - Pwm frequency must be betwen 1 and 1/2 Timer freq.
 * - Duty-cycle must be from 1 to 99
 * 
 * @param FreqHz 
 * @param DutyCyclePerc 
 * @param Activate 
 */
void DPwmOut::set(uint16_t freqHz, uint8_t dutyCyclePerc, bool activate) {
    if (!OUTPORT) {
        // Do nothing
        return;
    }

    currDutyPerc=dutyCyclePerc;

    // Pwm freq must from 1 to 1/2 Timer freq
    if (freqHz == 0) {
        low();
        return;
    }
    else if (freqHz > (TIMER_FREQ/2)) {
        // set to max
        freqHz=TIMER_FREQ/2;
    }
    // Total ticks per period
    ticksTOT=(TIMER_FREQ/freqHz);
    
    // Reset counter when setting new frequency
    tickCounter=0;

    setDutyPerc(dutyCyclePerc);

    active=activate;
}

/**
 * @brief Change the frequency of pwm.
 * 
 * @param FreqHz 
 */
void DPwmOut::setHz(uint16_t freqHz) {
   set(freqHz,currDutyPerc,true);
}

/**
 * @brief Change the pwm duty-cycle.
  * 
 * @param DutyCyclePerc ->  duty-cycle value in percentual (from 0 to 100):
 *                          1 to 99 :   generate pwm signal.
 *                          0       :   pin is put in LOW level.
 *                          >=100   :   pin is put in HIGH level.
 */
void DPwmOut::setDutyPerc(uint8_t dutyCyclePerc)
{
    if (!OUTPORT) {
        // Do nothing
        return;
    }

    if (ticksTOT == 0) {
        return;
    }

    currDutyPerc=dutyCyclePerc;
    if (currDutyPerc == 0) {
       low();
       return;
    }
    else if (currDutyPerc >= 100) {
        currDutyPerc=100;
        high();
        return;
    }
    // Number of ticks per pulse
    ticksON=(ticksTOT*currDutyPerc)/100;
    
    active=true;
}

/**
 * @brief Set duration of pwm pulse in microseconds.
 * N.B. Output pin wil be enabled.
 * 
 * @param Us 
 */
void DPwmOut::setMicros(uint16_t Us)
{
    if (!OUTPORT) {
        // Do nothing
        return;
    }
    
    if (Us == 0) {
        currDutyPerc=0;
        low();
    }
    else if (Us >= getPeriodUs()) {
        currDutyPerc=100;
        high();
    }
    else {
        ticksON=Us/US_PER_TICK;
        // ricalculate duty-cycle
        currDutyPerc=(100*ticksON)/ticksTOT;
        active=true;
    }
}

void DPwmOut::on(bool pwmOn) {
    if (pwmOn) {
        active=true;
    }
    else {
        low();
    }
}

/**
 * @brief Set still HIGH value on pin.
 * 
 */
void DPwmOut::high(void) {
    if (!OUTPORT) {
        // Do nothing
        return;
    }
    active=false;
    digitalWrite(pin,HIGH);
}

/**
 * @brief Set still LOW value on pin.
 * 
 */
void DPwmOut::low(void) {
    if (!OUTPORT) {
        // Do nothing
        return;
    }
    active=false;
    digitalWrite(pin,LOW);
}

/**
 * @return true if Pwm for this pin is ready to use.
 * N.B. Does not return the output enable status (use IsEnabled() instead).
 */
bool DPwmOut::isReady(void)
{
    if (OUTPORT) {
        return true;
    }

    return false;
}

/**
 * @return true if on Pin is present a pwm signal.
 * @return false if Pin is in HIGH or LOW state.
 */
bool DPwmOut::isActive(void) {
    return (active);
}

/**
 * @return uint16_t the period of pwm pulse.
 */
uint16_t DPwmOut::getPeriodUs(void)
{
    return(US_PER_TICK * ticksTOT);
}

/**
 * @return uint8_t pwm pin number.
 */
uint8_t DPwmOut::getPin(void)
{
    return(pin);
}

uint8_t DPwmOut::getDutyPerc(void) {
    return(currDutyPerc);
}

void DPwmOut::printInfo(void) {
    #if defined(HAVE_HWSERIAL0)
        noInterrupts();
        Serial.print("GpioPin ");Serial.println(pin);
        uint8_t Port=digitalPinToPort(pin);
        Serial.print("Port ");Serial.println(Port);
        Serial.print("BitMask ");Serial.println(bitMask);
        Serial.print("us per tick (resolution) ");Serial.println(US_PER_TICK);
        Serial.print("Ticks ON ");Serial.println(ticksON);
        Serial.print("Ticks TOT ");Serial.println(ticksTOT);
        Serial.print("Pin freq ");Serial.println(TIMER_FREQ/ticksTOT);
        Serial.print("Timer freq ");Serial.println(TIMER_FREQ);
        Serial.print("Max freq (duty only 50 perc) ");Serial.println(TIMER_FREQ/2);
        double f=static_cast<double>(F_CPU) / (OCR1A + 1);
        Serial.print("Interrupt frequency ");Serial.println(f);
        interrupts();
    #endif
}

#endif