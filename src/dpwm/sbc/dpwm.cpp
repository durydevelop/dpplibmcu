#include "dpwm.h"

/**
 * @brief Construct a new DPwmOut::DPwmOut object.
 * N.B. Timer should have not been initialized in costructor (may be it doe not work).
 * Timer will be initialized in Begin().
 */
DPwmOut::DPwmOut(int gpioPin, DGpioHandle gpioHandle) {
    pin=gpioPin;
    lastResult=DERR_CLASS_NOT_BEGUN;

    //std::cout << "DPwmOut() digitalPin=" << gpioPin<< " gpioHandle=" << gpioHandle << std::endl;

    if (gpioHandle < 0) {
        // Try init on first device
        handle=gpioHandle;
        //std::cout << "auto set handle" << std::endl;
        lastResult=initGpio(0,handle);
        if (lastResult > 0) {
            lastResult=DERR_CLASS_NOT_BEGUN;
        }
    }
    else {
        if (isGpioReady(gpioHandle)) {
            handle=gpioHandle;
        }
        else {
            lastResult=DERR_GPIO_NOT_READY;
        }
    }
}

/**
 * @brief Destroy the DPwmOut::DPwmOut object and free pin use.
 */
DPwmOut::~DPwmOut()
{
    releasePin(pin,handle);
}

/**
 * @brief Init all stuff for starting pwm on GpioPin.
 * MUST be calld before using the class.
 * 
 * @param GpioPin 
 * @param FreqHz 
 * @param DutyCyclePerc 
 * @param Activate 
 */
bool DPwmOut::begin(float frequecyHz, float dutyCyclePerc, bool activate) {
    dutyPerc=0.0;
    freqHz=0.0;
    active=false;

    if (handle >= 0) {
        lastResult=initPin(pin,DPinMode::PIN_MODE_SOFT_PWM,DPinFlags::NO_FLAGS,handle);
    }

    set(frequecyHz,dutyCyclePerc,activate);
    return lastResult == DRES_OK;
}

/**
 * @brief Set pwm frequency and duty-cycle.
 * 
 * @param freqHz frequency in Hz (0=off, 0.1-10000).
 * @param dutyCyclePerc PWM duty cycle in % (0-100).
 * @param activate if true, pwm pulse will be out.
 */
bool DPwmOut::set(float frequecyHz, float dutyCyclePerc, bool activate) {
    //std::cout << "lgTxPwm=" << freqHz <<  "dutyPerc=" << dutyCyclePerc << std::endl;
    // Pwm freq must from 1 to 1/2 Timer freq
    if (frequecyHz > MAX_PWM_FREQ) {
        // set to max
        freqHz=MAX_PWM_FREQ;
    }
    else {
        freqHz=frequecyHz;
    }

    if (dutyCyclePerc <= 0) {
        dutyPerc=0;
    }
    else if (dutyCyclePerc >= 100) {
        dutyPerc=100;
    }
    else {
        dutyPerc=dutyCyclePerc;
    }

    active=activate;
    if (activate) {
        lastResult=lgTxPwm(handle,pin,freqHz,dutyPerc,0,0);
        if (lastResult < 0) {
            active=false;
        }
    }

    return active;
}

/**
 * @brief Set the frequency of pwm and activate pwm.
 * 
 * @param FreqHz frequency in Hz (0=off, 0.1-10000)
 */
bool DPwmOut::setHz(float frequecyHz) {
   return set(freqHz,dutyPerc,true);
}

/**
 * @brief Set the pwm duty-cycle and activate pwm.
  * 
 * @param DutyCyclePerc ->  duty-cycle value in percentual (from 0 to 100):
 *                          1 to 99 :   generate pwm signal.
 *                          0       :   pin is put in LOW level.
 *                          >=100   :   pin is put in HIGH level.
 */
bool DPwmOut::setDutyPerc(float dutyCyclePerc)
{
    return set(freqHz,dutyCyclePerc,true);
}

/**
 * @brief Set duration of pwm pulse in microseconds.
 * If pulse is greater than current period, duty cycle wil be set 100%.
 * The frequency is 1000000 / (pulseOn us + pulseOff us) Hz, so:
 * pulseOff us = (1000000 / frequency hz) - pulseOn us
 * 
 * The set duty cycle will be pulseOn / (pulseOn + pulseOff) * 100 %
 * 
 * @param us 
 */
bool DPwmOut::setMicros(uint16_t us)
{
    float usOff=0;

    if (us == 0) {
        dutyPerc=0;
    }
    else if (us >= getPeriodUs()) {
        dutyPerc=100;
    }
    else {
        usOff=(1000000 / freqHz) - us;
        // Ricalculate duty-cycle
        dutyPerc=us / (us+usOff) * 100;
    }

    //lastRet=lgTxPwm(handle,pin,freqHz,dutyPerc,0,0);
    // Should be same as:
    lastResult=lgTxPulse(handle,pin,us,usOff,0,0);

    return lastResult == DRES_OK;
}

bool DPwmOut::on(bool PwmOn) {
    if (PwmOn) {
        set(pin,freqHz,true);
    }
    else {
        lastResult=lgTxPwm(handle,pin,0,0,0,0);
    }

    return lastResult == DRES_OK;
}

/**
 * @brief Set still HIGH value on pin.
 * 
 */
bool DPwmOut::high(void) {
    lastResult=writePin(pin,HIGH,handle);
    return lastResult == DRES_OK;
}

/**
 * @brief Set still LOW value on pin.
 * 
 */
bool DPwmOut::low(void) {
    lastResult=writePin(pin,LOW,handle);
    return lastResult == DRES_OK;
}

/**
 * @return true if Pwm for this pin is ready to use.
 * N.B. Does not return the output enable status (use IsEnabled() instead).
 */
bool DPwmOut::isReady(void)
{
    return lastResult == DRES_OK;
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
    float period=1/freqHz;
    return (uint16_t)(period * 1e6);
}

/**
 * @return uint8_t pwm pin number.
 */
uint8_t DPwmOut::getPin(void)
{
    return(pin);
}

uint8_t DPwmOut::getDutyPerc(void) {
    return(dutyPerc);
}

std::string DPwmOut::getLastError(void) {
    return getErrorCode(lastResult);
}
