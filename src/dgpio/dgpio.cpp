#include "dgpio.h"

/**
 * @brief Initilize gpio chip.
 * - Need to be called once.
 * - initPin() can call it by itself.
 * 
 * @param gpioDevice    ->  device index.
 * @param handle        ->  (not for Arduino) the handle obtained from initGpio() or DGpioChip class.
 * @return DRES_OK on success otherwise a DResult error code (you can call getErrorCode(result) for retriving error string).
 * On ARDUINO return always DRES_OK.
 */
#ifdef ARDUINO
DResult initGpio(int gpioDevice)
{
    return DRES_OK;
}
#else
DResult initGpio(int gpioDevice, DGpioHandle& handle)
{
    handle = lgGpiochipOpen(gpioDevice);
    if (handle < 0) {
        fatal("Cannot continue: lgGpiochipOpen() failed with error: ",handle);
    }
    return handle;
}
#endif

/**
 * @brief Check if gpio chip is initilized.
 * On ARDUINO return always true;
 * 
 * @param handle    ->  (not for Arduino) the handle obtained from initGpio() or DGpioChip class.
 * @return true on success otherwise false.
 */
#ifdef ARDUINO
bool isGpioReady(void)
{
    return true;
}
#else
bool isGpioReady(DGpioHandle handle)
{
    if (handle < 0) {
        return false;
    }
    lgChipInfo_t cInfo;
    int status = lgGpioGetChipInfo(0, &cInfo);
    if (status == LG_OKAY) {
        return true;
    }
    return false;
}
#endif


/**
 * @brief Configures a pin to be used as input, output or whatever.
 * 
 * @param pin       ->  gpio pin.
 * @param mode      ->  one of DPinMode values.
 * @param flags     ->  on of DPinFlags values.
 * @param handle    ->  (not for Arduino) the handle obtained from initGpio() or DGpioChip class.
 * @return DRES_OK on success otherwise a DResult error code (you can call getErrorCode(result) for retriving error string).
 */
#ifdef ARDUINO
DResult initPin(uint8_t pin, DPinMode mode, DPinFlags flags)
{
        if (mode == DPinMode::PIN_MODE_SOFT_PWM) {
            #ifdef USE_DSOFT_PWM
                /// @todo dPwmOut=new DPwmOut(Pin);
            #else
                pinMode(pin,OUTPUT);
            #endif
        }
        else {
            pinMode(pin,mode);
        }
        return DRES_OK;
}
#else
DResult initPin(uint8_t pin, DPinMode mode, DPinFlags flags, DGpioHandle handle)
{
        if (handle < 0) {
            // Try init on first device
            initGpio(0,handle);
        }
        
        // Set mode
        int ret=DERR_UNKOWN_PIN_MODE;
        switch (mode) {
            case DPinMode::PIN_MODE_INPUT:
                ret=lgGpioClaimInput(handle,flags,pin);
                break;
            case DPinMode::PIN_MODE_OUTPUT:
            case DPinMode::PIN_MODE_SOFT_PWM:
                ret=lgGpioClaimOutput(handle,flags,pin,0);
                break;
            case DPinMode::PIN_MODE_INPUT_PULLUP:
                ret=lgGpioClaimInput(handle,DPinFlags::PULL_UP,pin);
                break;
            default:
                break;
        }
        return ret;
}
#endif

/**
 * @brief Read pin level.
 * 
 * @param Pin       -> gpio pin.
 * @param handle    ->  (not for Arduino) the handle obtained from initGpio() or DGpioChip class.
 * @return pin level HIGH or LOW (negative number is errCode)
 */
#ifdef ARDUINO
int readPin(uint8_t pin)
{
    return digitalRead(pin);
}
#else
int readPin(uint8_t pin, DGpioHandle handle)
{
    return lgGpioRead(handle,pin);
}
#endif

/**
 * @brief Set pin level.
 * 
 * @param pin       ->  gpio pin.
 * @param level     ->  level: HOGH or LOW.
 * @param handle    ->  (not for Arduino) the handle obtained from initGpio() or DGpioChip class.
 * @return DRES_OK on success otherwise a DResult error code (you can call getErrorCode(result) for retriving error string).
 */
#ifdef ARDUINO
DResult writePin(uint8_t pin, uint8_t level)
{
    digitalWrite(pin,level);
    return DRES_OK;
}
#else
DResult writePin(uint8_t pin, uint8_t level, DGpioHandle handle)
{
    return lgGpioWrite(handle,pin,level);
}
#endif

/**
 * @brief Output a pwm signal to pin.
 * 
 * ARDUINO:
 * PWM output only works on the pins with hardware support.
 * See https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/ for details.
 * 
 * @param pin       ->  gpio pin.
 * @param Value     ->  pwm value from 0 to 255.
 * @param handle    ->  (not for Arduino) the handle obtained from initGpio() or DGpioChip class.
 * @return DRES_OK on success otherwise a DResult error code (you can call getErrorCode(result) for retriving error string).
 */
#ifdef ARDUINO
DResult writeAnalog(uint8_t pin, uint8_t value)
{
    analogWrite(pin,value);
    return DRES_OK;
}
#else
DResult writeAnalog(uint8_t pin, uint8_t value, DGpioHandle handle)
{
    float dutyCycle=mapValue(value,0,255,0,100);
    return lgTxPwm(handle,pin,500,dutyCycle,0,0);
}
#endif

/**
 * @brief Free a pin that can be claimed for an other use.
 * 
 * @param pin       ->  gpio pin.
 * @param handle    ->  (not for Arduino) the handle obtained from initGpio() or DGpioChip class.
 * @return DRES_OK on success otherwise a DResult error code (you can call getErrorCode(result) for retriving error string).
 */
#ifdef ARDUINO
DResult releasePin(uint8_t pin)
{
    return DRES_OK;
}
#else
DResult releasePin(uint8_t pin, DGpioHandle handle)
{
    return lgGpioFree(handle,pin);
}
#endif

/**
 * @brief Close handle for gpio chip.
 * On ARDUINO return true;
 * 
 * @param handle    ->  (not for Arduino) the handle obtained from initGpio() or DGpioChip class.
 * @return DRES_OK on success otherwise a DResult error code (you can call getErrorCode(result) for retriving error string).
 */
#ifdef ARDUINO
DResult shutdownGpio(void)
{
    return DRES_OK;
}
#else
DResult shutdownGpio(DGpioHandle handle)
{
    return lgGpiochipClose(handle);
}
#endif