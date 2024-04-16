#include <Arduino.h>
#include "dpwm.h"
#include "DSoftPwmTimer.h"


/**
 * TODO:
 * 
*/


#define MAX_PWM_COUNT 5

//static std::vector<DPwm*> ChList;  //! List of Pwm channel
DPwmOut *PwmList[MAX_PWM_COUNT];
bool Initialized=false;

#if defined(HAVE_HWSERIAL0)
    #define Debug(s) Serial.println(s)
#else
    #define Debug(s)
#endif

// ********************************************************************************
// ********************************* ISR routine **********************************
// ********************************************************************************
size_t TickCounter=0;
#if defined(__AVR_ATtiny85__)
    ISR(TIMER0_COMPA_vect)
#elif defined (__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    ISR(TIMER1_COMPA_vect)
#endif
{
    for (uint8_t ixC=0; ixC<MAX_PWM_COUNT; ixC++) {
        if (PwmList[ixC] ) {
            if (PwmList[ixC]->Active) {
                PwmList[ixC]->TickCounter++;
                if (PwmList[ixC]->TickCounter >= PwmList[ixC]->TicksTOT) {
                    // Pwm period end, set to HIGH
                    *PwmList[ixC]->OUTPORT|=PwmList[ixC]->BitMask;
                    PwmList[ixC]->TickCounter=0;
                }
                else if (PwmList[ixC]->TickCounter >= PwmList[ixC]->TicksON) {
                    // Pwm HIGH level time reached, change to LOW
                    *PwmList[ixC]->OUTPORT&=~PwmList[ixC]->BitMask;
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
DPwmOut::DPwmOut(uint8_t GpioPin) {
    // Set defaults
    Pin=GpioPin;
    BitMask=0;
    OUTPORT=nullptr;
    Active=false;
    TickCounter=0;
    TicksON=0;
    TicksTOT=0;
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
bool DPwmOut::begin(uint16_t FreqHz, uint8_t DutyCyclePerc, bool Activate) {
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
        Debug("No free slot");
        return false;
    }

    // Get port register for pin
    uint8_t Port=digitalPinToPort(Pin);
    if (Port == NOT_A_PIN) {
        Debug("Not a pin");
        return false;
    }

    // Set all stuff
    pinMode(Pin,OUTPUT);
    BitMask=digitalPinToBitMask(Pin);
    OUTPORT=portOutputRegister(Port);
    set(FreqHz,DutyCyclePerc,Activate);
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

    dutyPerc=dutyCyclePerc;

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
    TicksTOT=(TIMER_FREQ/freqHz);

    setDutyPerc(dutyCyclePerc);

    Active=activate;
}

/**
 * @brief Change the frequency of pwm.
 * 
 * @param FreqHz 
 */
void DPwmOut::setHz(uint16_t FreqHz) {
   set(FreqHz,dutyPerc,true);
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

    if (TicksTOT == 0) {
        return;
    }

    dutyPerc=dutyCyclePerc;
    if (dutyPerc == 0) {
       low();
       return;
    }
    else if (dutyPerc >= 100) {
        dutyPerc=100;
        high();
        return;
    }
    // Number of ticks per pulse
    TicksON=(TicksTOT*dutyPerc)/100;
    
    Active=true;
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
        dutyPerc=0;
        low();
    }
    else if (Us >= getPeriodUs()) {
        dutyPerc=100;
        high();
    }
    else {
        TicksON=Us/US_PER_TICK;
        // ricalculate duty-cycle
        dutyPerc=(100*TicksON)/TicksTOT;
        Active=true;
    }
}

void DPwmOut::on(bool PwmOn) {
    if (PwmOn) {
        Active=true;
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
    Active=false;
    digitalWrite(Pin,HIGH);
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
    Active=false;
    digitalWrite(Pin,LOW);
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
    return (Active);
}

/**
 * @return uint16_t the period of pwm pulse.
 */
uint16_t DPwmOut::getPeriodUs(void)
{
    return(US_PER_TICK * TicksTOT);
}

/**
 * @return uint8_t pwm pin number.
 */
uint8_t DPwmOut::getPin(void)
{
    return(Pin);
}

uint8_t DPwmOut::getDutyPerc(void) {
    return(dutyPerc);
}

void DPwmOut::printInfo(void) {
    #if defined(HAVE_HWSERIAL0)
        noInterrupts();
        Serial.print("GpioPin ");Serial.println(Pin);
        uint8_t Port=digitalPinToPort(Pin);
        Serial.print("Port ");Serial.println(Port);
        Serial.print("BitMask ");Serial.println(BitMask);
        Serial.print("us per tick (resolution) ");Serial.println(US_PER_TICK);
        Serial.print("Ticks ON ");Serial.println(TicksON);
        Serial.print("Ticks TOT ");Serial.println(TicksTOT);
        Serial.print("Pin freq ");Serial.println(TIMER_FREQ/TicksTOT);
        Serial.print("Timer freq ");Serial.println(TIMER_FREQ);
        Serial.print("Max freq (duty only 50 perc) ");Serial.println(TIMER_FREQ/2);
        double f=static_cast<double>(F_CPU) / (OCR1A + 1);
        Serial.print("Interrupt frequency ");Serial.println(f);
        interrupts();
    #endif
}
