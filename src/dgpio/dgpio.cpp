#include "dgpio.h"
#include <derrorcodes.h>

/**
 * @brief Initilize gpio chip.
 * - Must be call once before any gpio operation.
 * - InitPin() call it by it self
 * - On ARDUINO return true.
 * 
 * @param gpioDevice    ->  device index.
 * @param DGpioHandle   ->  handle from initGpio(), on ARDUINO is ignored.
 * @return true on success otherwise false.
 */
DResult initGpio(int gpioDevice, DGpioHandle& handle)
{
    #ifdef ARDUINO
        return DRES_OK;
    #elif defined LGPIO_VERSION
        if (handle < 0) {
            handle = lgGpiochipOpen(gpioDevice);
            if (handle < 0) {
                fatal("Cannot continue: lgGpiochipOpen() failed with error: ",handle);
            }
            return handle;
        }
    #elif defined PGPIO_VERSION
        if (gpioInitialise() == PI_INIT_FAILED) {
            return false;
        }
    #endif
    return true;
}


/**
 * @brief Check if gpio chip is initilized.
 * On ARDUINO return true;
 * 
 * @param DGpioHandle   ->  handle from initGpio(), on ARDUINO is ignored.
 * @return true on success otherwise false.
 */
bool isGpioReady(DGpioHandle& handle)
{
    #ifdef ARDUINO
        return true;
    #elif defined LGPIO_VERSION
        if (handle < 0) {
            return false;
        }
        lgChipInfo_t cInfo;
        int status = lgGpioGetChipInfo(0, &cInfo);
        if (status == LG_OKAY) {
            return true;
        }
        return false;
    #elif defined PGPIO_VERSION
        if (gpioInitialise() == PI_INITIALISED) {
            return true;
        }
        return false;
    #endif
}


/**
 * @brief Configures a pin to be used as input, output or whatever.
 * 
 * @param pin           ->  gpio pin.
 * @param mode          ->  one of DPinMode values.
 * @param flags         ->  on of DPinFlags values.
 * @param DGpioHandle   ->  handle from initGpio(), on ARDUINO is ignored.
 * @return true on success otherwise false.
 */
DResult initPin(uint8_t pin, DPinMode mode, DPinFlags flags, DGpioHandle handle)
{
    #ifdef ARDUINO
        if (Mode == DPinMode::SOFT_PWM) {
            #ifdef USE_DSOFT_PWM
                // TODO: dPwmOut=new DPwmOut(Pin);
            #else
                pinMode(Pin,OUTPUT);
            #endif
        }
        else {
            pinMode(Pin,Mode);
        }
        return DRES_OK;
    #elif defined LGPIO_VERSION
        if (handle < 0) {
            // Try init on first device
            initGpio(0,handle);
        }
        
        // Set mode
        int ret=DERR_UNKOWN_PIN_MODE;
        switch (mode) {
            case DPinMode::INPUT:
                ret=lgGpioClaimInput(handle,flags,pin);
                break;
            case DPinMode::OUTPUT:
            case DPinMode::SOFT_PWM:
                ret=lgGpioClaimOutput(handle,flags,pin,0);
                break;
            case DPinMode::INPUT_PULLUP:
                ret=lgGpioClaimInput(handle,DPinFlags::PULL_UP,pin);
                break;
            default:
                break;
        }
        return ret;
    #elif defined PIGPIO_VERSION
        // TODO: deprecate
        if (Mode == OUTPUT_PWM_MODE) {
            gpioSetMode(Pin,PI_OUTPUT);
        }
        else {
            gpioSetMode(Pin,Mode);
        }
        return DRES_OK;
    #endif
}

/**
 * @brief Read pin level.
 * 
 * @param Pin           -> gpio pin.
 * @param DGpioHandle   ->  handle from initGpio(), on ARDUINO is ignored.
 * @return pin level HIGH or LOW (negative number is errCode)
 */
int readPin(uint8_t pin, DGpioHandle handle)
{
    #ifdef ARDUINO
        return digitalRead(pin);
    #elif defined LGPIO_VERSION
        return lgGpioRead(handle,pin);
    #elif defined PIGPIO_VERSION
        // TODO: deprecate
        return gpioRead(Pin);
    #endif
}

/**
 * @brief Set pin level.
 * 
 * @param pin           ->  gpio pin.
 * @param level         ->  level: HOGH or LOW.
 * @param DGpioHandle   ->  handle from initGpio(), on ARDUINO is ignored.
 * @return true on success otherwise false.
 */
DResult writePin(uint8_t pin, uint8_t level, DGpioHandle handle)
{
    #ifdef ARDUINO
		digitalWrite(Pin,Level);
        return DRES_OK
    #elif defined LGPIO_VERSION
        return lgGpioWrite(handle,pin,level);
    #elif defined PIGPIO_VERSION
        // TODO: deprecate
        gpioWrite(Pin,Level);
        return DRES_OK;
    #endif
}

/**
 * @brief Output a pwm signal to pin.
 * 
 * ARDUINO:
 * PWM output only works on the pins with hardware support.
 * See https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/ for details.
 * 
 * @param pin           ->  gpio pin.
 * @param Value         ->  pwm value from 0 to 255.
 * @param DGpioHandle   ->  handle from initGpio(), on ARDUINO is ignored.
 * @return true on success otherwise false.
 */
DResult writeAnalog(uint8_t pin, uint8_t value, DGpioHandle handle)
{
    #ifdef ARDUINO
        analogWrite(pin,value);
        return DRES_OK;
    #elif defined LGPIO_VERSION
        float dutyCycle=mapValue(value,0,255,0,100);
        return lgTxPwm(handle,pin,500,dutyCycle,0,0);
    #elif defined PIGPIO_VERSION
        // TODO: deprecate
        gpioPWM(Pin,Value);
        return DRES_OK;
    #endif
}

/**
 * @brief Free a pin that can be claimed for an other use.
 * On ARDUINO return true;
 * 
 * @param pin           ->  gpio pin.
 * @param DGpioHandle   ->  handle from initGpio(), on ARDUINO is ignored.
 * @return true on success otherwise false.
 */
DResult releasePin(uint8_t pin, DGpioHandle handle)
{
    #ifdef ARDUINO
        return DRES_OK;
    #elif defined LGPIO_VERSION
        return lgGpioFree(handle,pin);
    #elif defined PIGPIO_VERSION
        return DRES_OK;
    #endif
}

/**
 * @brief Close handle for gpio chip.
 * On ARDUINO return true;
 * 
 * @param DGpioHandle   ->  handle from initGpio(), on ARDUINO is ignored.
 * @return true on success otherwise false.
 */

DResult shutdownGpio(DGpioHandle handle)
{
    #ifdef ARDUINO
        return DRES_OK;
    #elif defined LGPIO_VERSION
        return lgGpiochipClose(handle);
    #elif defined PGPIO_VERSION
        gpioTerminate();
        return DRES_OK;
    #endif
}