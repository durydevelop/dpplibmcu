#include "dgpio.h"

#ifndef ARDUINO
    #include <iostream>

    /**
     * @brief Initilize gpio chip.
     * - Must be call once before any gpio operation.
     * - InitPin() call it by it self
     * 
     * @param gpioDevice device index.
     * @return true on success otherwise false.
     */
    bool initGpio(int gpioDevice) {
        chip = lgGpiochipOpen(gpioDevice);
        if (chip < 1) {
            // TODO: return false or exit?
            std::cerr << "Cannot continue: lgGpiochipOpen() failed with error " << chip << std::endl;
            exit(-1);
        }
        return true;
    }

    /**
     * @brief Close handle for gpio chip.
     * 
     * @param gpioDevice device index.
     * @return true on success otherwise false.
     */
    bool shutdownGpio(int gpioDevice) {
        if (chip >= 0) {
            return lgGpiochipClose(chip);
        }
        return true;
    }

#endif

bool initPin(uint8_t pin, DPinMode mode, DPinFlags flags)
{
    int ret=0; // default return ok

    #ifdef ARDUINO
        if (Mode == OUTPUT_PWM_MODE) {
            #ifdef USE_DSOFT_PWM
                // TODO: dPwmOut=new DPwmOut(Pin);
            #else
                pinMode(Pin,OUTPUT);
            #endif
        }
        else {
            pinMode(Pin,Mode);
        }
    #else
        // Not arduino (rpi or linux sbc)
        #ifdef LGPIO_VERSION
            // Init Library
            initGpio();
            // Set mode
            switch (mode) {
                case DPinMode::INPUT:
                    ret=lgGpioClaimInput(chip,flags,pin);
                    break;
                case DPinMode::OUTPUT:
                //case DPinMode::OUTPUT_PWM:
                    ret=lgGpioClaimOutput(chip,flags,pin,0);
                    break;
                case DPinMode::INPUT_PULLUP:
                    // Arduino compatibility
                    ret=lgGpioClaimInput(chip,DPinFlags::PULL_UP,pin);
                    break;
                default:
                    return false;
            }
        #else //#ifdef PIGPIO_VERSION
            // TODO: deprecate
            // Init lib
            if (gpioInitialise() == PI_INIT_FAILED) {
                return false;
            }

            // Set mode
            if (Mode == OUTPUT_PWM_MODE) {
                gpioSetMode(Pin,PI_OUTPUT);
            }
            else {
                gpioSetMode(Pin,Mode);
            }
        #endif
    #endif

    if (ret == 0) {
        return true;
    }
    return false;
}

/**
 * @brief Read pin level.
 * 
 * @param Pin 
 * @return pin level (HIGH or LOW)
 */
uint8_t readPin(uint8_t pin) {
    #ifdef ARDUINO
		return(digitalRead(Pin));
    #else
        // Not arduino (rpi or linux sbc)
        #ifdef LGPIO_VERSION
           return lgGpioRead(chip,pin);
        #else //#ifdef PIGPIO_VERSION
            // TODO: deprecate
			return(gpioRead(Pin));
        #endif
    #endif
}

bool writePin(uint8_t pin, uint8_t level)
{
    #ifdef ARDUINO
		digitalWrite(Pin,Level);
        return true;
    #else
        #ifdef LGPIO_VERSION
            if (lgGpioWrite(chip,pin,level) == 0) {
                return true;
            }
            return false;
        #else //#ifdef PIGPIO_VERSION
            // TODO: deprecate
			gpioWrite(Pin,Level);
            return true;
        #endif
    #endif
}

/**
 * @brief Output a pwm signal to pin.
 * 
 * ARDUINO:
 * For now:
 * PWM output only works on the pins with hardware support.
 * See https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/ for details.
 * 
 * @param Pin    ->  pwm pin
 * @param Value  ->  pwm value from 0 to 255
 */
bool writeAnalog(unsigned short int pin, unsigned short int value)
{
    #ifdef ARDUINO
        analogWrite(Pin,Value);
        return true;
    #else
        #ifdef LGPIO_VERSION
            float dutyCycle=map(value,0,255,0.0,100.0);
            if (lgTxPwm(chip,pin,500,dutyCycle,0,0) == 0) {
                return true;
            }
            return false;
        #else //#ifdef PIGPIO_VERSION
            // TODO: deprecate
            gpioPWM(Pin,Value);
            return true;
        #endif
    #endif
}
