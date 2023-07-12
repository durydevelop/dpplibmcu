/*! \mainpage 
*
* \section intro_sec Introduction
*
* \section usage_sec Usage
*
* @code

#include <Arduino.h>
#include <dstepper.h>

DStepper dStepper(0,2); // Use Pin o for Direction and pin 2 for clock

void setup() {
    dStepper.begin();        // Need to call begin to init timers out of constructor
    dStepper.Ramp(120,4000); // Reach 120 rpm in 4 seconds
    dStepper.Ramp(60,2000);  // Decrease to 60 rpm in 2 seconds
    dStepper.Ramp(0,2000);   // Stop motor in 2 seconds
}

void loop() {
}

* @endcode
*/

#include <dpplib-gpio.h>
#include "DStepper.h"

/**
 * TODO:
 *      _GetMaxRpm()
 *      _Step()
 *      _Optional Enable pin
 *      _Ramp StopPin
 *      _RampFreq
 */

/** Create a StepperMotorDrv Object
*
* @param PinDir         ->  Digital Output Pin to set Direction of motor
* @param PinClk         ->  Pwm Output Pin for Clock signal
* @param StepsPerRev    ->  Number of motor step for each revolution. If your motor gives the number
*                           of degrees per step, divide that number into 360 to get the number of steps (e.g. 360 / 1.6 gives 200 steps).
*                           Default value 200 (1.6 degree per step).
*/
DStepper::DStepper(uint8_t PinDir, uint8_t PinClk, uint16_t StepsPerRev) :
DirPin(PinDir),
ClkPin(PinClk)
{
    // Init direction pin
    InitPin(DirPin,OUTPUT_MODE);
    SetDir(ROT_CW);
    // Create software pwm for clock pin
    dPwmOut=new DPwmOut(ClkPin);
    
    // Default values
    StepsPerRevol=StepsPerRev;
    SwappedDir=0;
    CurrFreqHz=0;
    CurrDutyCycle=50;
}

/**
 * @brief Destroy the DStepper::DStepper object
 */
DStepper::~DStepper()
{
    delete dPwmOut;
}

/**
 * @brief Initialize all needed stuffs to start lib (and do nothing else).
 * Initialize DSoftPwm lib used to generate pwm signal.
 * The pwm signal use mcu timer and ISR (interrupt service routine), but ISR cannot be initialized inside constructors (make some strange things timing mistakes),
 * so, must be call after contructor to make DStepper lib to work.
  */
void DStepper::begin(void)
{
    dPwmOut->begin();
}

/**
 * @brief Initialize all needed stuffs to start lib, set clock frequency and enable it on pin (if required).
 * Initialize DSoftPwm lib used to generate pwm signal.
 * The pwm signal use mcu timer and ISR (interrupt service routine), but ISR cannot be initialized inside constructors (make some strange things timing mistakes),
 * so, must be call after contructor to make DStepper lib to work.
 * 
 * @param FreqHz    ->  Frequency in Hz.
 * @param Enabled   ->  If true, pwm is enable on clock pin.
 */
void DStepper::begin(uint16_t FreqHz, bool Enabled)
{
    dPwmOut->begin();
    SetFreq(FreqHz);
    if (Enabled) {
        dPwmOut->On();
    }
}

/**
 * @brief Initialize all needed stuffs to start lib, set rpm value and enable it on pin (if required).
 * Initialize DSoftPwm lib used to generate pwm signal.
 * The pwm signal use mcu timer and ISR (interrupt service routine), but ISR cannot be initialized inside constructors (make some strange things timing mistakes),
 * so, must be call after contructor to make DStepper lib to work.
 * 
 * @param Rpm       ->  Rpm value used to calulate clock frequency.
 * @param Enabled   ->  If true, pwm is enable on clock pin.
 */
void DStepper::begin(uint8_t Rpm, bool Enabled)
{
    dPwmOut->begin();
    SetRpm(Rpm);
    if (Enabled) {
        dPwmOut->On();
    }
}

// **********************************************************************************************************************
// ************************************************* CONFIGURATION API **************************************************
// **********************************************************************************************************************

/**
 * @brief Invert level on PinDir when setting rotation direction.
 * 
 * @param Enabled   ->  if true, direction handle is inverted.
 */
void DStepper::SetSwappedDir(bool Enabled)
{
    if (Enabled) {
        SwappedDir=1;
    }
    else {
        SwappedDir=0;
    }
}

/**
 * @brief Set number of motor step for each revolution.
 * If your motor gives the number of degrees per step, divide that number into 360 to get the number of steps (e.g. 360 / 1.6 gives 200 steps).
 * 
 * @param Steps the number of steps in one revolution of your motor.
 */
void DStepper::SetStepsPerRevolution(uint16_t Steps)
{
    StepsPerRevol=Steps;
}

/**
 * @brief Set current pwm duty-cycle for clock pin.
 * 
 * @param DutyCyclePerc ->  Duty-cycle value in percentual from 0 to 100.
 *                          Value of 0 put pin always LOW.
 *                          Value of 100 put clck pin always HIGH.
 */
void DStepper::SetClkDutyCycle(uint8_t DutyCyclePerc)
{
    CurrDutyCycle=DutyCyclePerc;
}

// **********************************************************************************************************************
// **************************************************** GETTERS API *****************************************************
// **********************************************************************************************************************

/**
 * @brief Get current frequency on clock pin.
 * 
 * @return uint16_t Frequency in Hz.
 */
uint16_t DStepper::GetClkFreq(void)
{
    return(CurrFreqHz);
}

/**
 * @brief Get current duty-cycle used for clock pin.
 * 
 * @return uint8_t  ->  Duty-cycle value in percentual.
 */
uint8_t DStepper::GetClkDutyCycle(void)
{
    return(CurrDutyCycle);
}
/**
 * @brief Get current rotation direction.
 * 
 * @return ROT_CW for clocwWise direction, ROT_CC for counter-clockwise direction.
 */
DStepper::DRotationDir DStepper::GetDir(void)
{
    return((DStepper::DRotationDir) (ReadPin(DirPin) ^ SwappedDir));
}

/**
 * @brief Get current rpm value.
 * 
 * @return uint16_t rpm value.
 */
uint16_t DStepper::GetRpm(void)
{
    return((CurrFreqHz*60)/StepsPerRevol);
}

// **********************************************************************************************************************
// ********************************************* DIRECT MOTOR CONTROL API ***********************************************
// **********************************************************************************************************************

/**
 * @brief Enable pwm output, this starts the motor at CurrFreqHz and CurrDutyCycle values (output directly on pin).
 */
void DStepper::On(void)
{
    dPwmOut->On();
}

/**
 * @brief Disable pwm output, this stops the motor (output directly on pin).
 */
void DStepper::Off(void)
{
    dPwmOut->Off();
}

/**
 * @brief Set speed (output directly on pin).
 * 
 * @param Rpm   ->  Rpm value to set.
 */
void DStepper::SetRpm(uint16_t Rpm)
{
    CurrFreqHz=(Rpm*StepsPerRevol)/60;
    dPwmOut->SetFreq(CurrFreqHz,CurrDutyCycle);
}

/**
 * @brief Set speed and direction (output directly on pin).
 * 
 * @param Rpm           ->  Rpm value to set.
 * @param RotationDir   ->  ROT_CC for counter clockwise or ROT_CW for clockwise.
 * N.B. RotationDir will be inverted if SwapDirection is set to 1.
 */
void DStepper::SetRpm(uint16_t Rpm, DStepper::DRotationDir RotationDir)
{
    CurrFreqHz=(Rpm*StepsPerRevol)/60;
    dPwmOut->SetFreq(CurrFreqHz,CurrDutyCycle);
    SetDir(RotationDir);
}

/**
 * @brief Set frequency to apply on clock pin.
 * 
 * @param FreqHz        ->  Frequency in Hz to out on clocl pin.
 */
void DStepper::SetFreq(uint16_t FreqHz)
{
    CurrFreqHz=FreqHz;
    dPwmOut->SetFreq(CurrFreqHz,CurrDutyCycle);
}

/**
 * @brief Set frequency to apply on clock pin and rotation direction.
 * 
 * @param FreqHz        ->  Frequency in Hz to out on clocl pin.
 * @param RotationDir   ->  ROT_CC for counter clockwise or ROT_CW for clockwise.
 */
void DStepper::SetFreq(uint16_t FreqHz, DRotationDir RotationDir)
{
    CurrFreqHz=FreqHz;
    dPwmOut->SetFreq(CurrFreqHz,CurrDutyCycle);
    SetDir(RotationDir);
}

/**
 * @brief Set frequency and duty-cycle to apply on clock pin.
 * 
 * @param FreqHz        ->  Frequency in Hz to out on clocl pin.
 * @param DutyCyclePerc ->  CC for counter clockwise or CW for clockwise.
 */
void DStepper::SetFreq(uint16_t FreqHz, uint8_t DutyCyclePerc)
{
    CurrFreqHz=FreqHz;
    CurrDutyCycle=DutyCyclePerc;
    dPwmOut->SetFreq(CurrFreqHz,CurrDutyCycle);
}

/**
 * @brief Reach Rpm speed using ramp (output directly on pin).
 * Change speed from CurrRpm to Rpm in time of RampMillis.
 * N.B.:
 * -Ramp is calculated for both increase and decrese speed.
 * -Motor rotation direction still remain to this RotationDir value.
 * -One pin is used as emergency stop.
 * 
 * e.g. 
 * @code {.C++}
    // If Pin 2 wil trigger a LOW_LEVEL status funcion stops before Ramp time.
    bool fault=Ramp(120,4000,CC,2,LOW_LEVEL); // Reach 120 rpm in 4 seconds
    if (!fault) {
        Ramp(60,2000,CC,2,LOW_LEVEL); // Decrease to 60 rpm in 2 seconds
    }
    if (!fault) {
        Ramp(0,2000,CC,2,LOW_LEVEL); // Stop motor in 2 seconds
    }
 * @endcode
 * 
 * @param RampMillis    ->  Duration of ramp in milliseconds. Setting a value if 0, immedialtly apply Rpm value.
 * @param Rpm           ->  Rpm value to set.
 * @param RotationDir   ->  Specify rotation direction ROT_CC or ROT_CW. ROT_CURRENT will not change it.
 * @param FaultPin      ->  Pin used to emergency stop motor, when this pin is in FaultLevel, motor will be stopped.
 *                          A value of -1 disable the check (!! BE CAREFULLY !!).
 * @param FaultLevel    ->  The FaultPin level that triggers the emergency stop.
 * 
 * @return false if FaultPin has triggered an emergency stop, otherwise, after RampMillis time, return true.
 */
bool DStepper::RampRpm(uint16_t RampMillis, uint16_t Rpm, DRotationDir RotationDir, int8_t FaultPin, DPinLevel FaultLevel)
{
    if (RampMillis > 0) {
        unsigned long StartRamp=millis();
        uint16_t CurrRpm=GetRpm();
        int8_t Inc=1; // default increasing
        if (Rpm < CurrRpm) {
            // decreasing
            Inc=-1;
        }

        uint16_t DelayMillis=RampMillis/((Rpm-CurrRpm)*Inc); // Delay time after each rpm increment
        if (RotationDir != ROT_CURRENT) {
            SetDir(RotationDir);
        }
        dPwmOut->On();
        while (millis()-StartRamp < RampMillis) {
            CurrRpm+=Inc;
            dPwmOut->SetFreq((CurrRpm*StepsPerRevol)/60,CurrDutyCycle);
            unsigned long StartDelay=millis();
            while (millis()-StartDelay < DelayMillis) {
                if (FaultPin >= 0) {
                    if (ReadPin(FaultPin) == FaultLevel) {
                        dPwmOut->Off();
                        return false;
                    }
                }
            };
        }
    }
    SetRpm(Rpm);
    return true;
}

/**
 * @brief Set direction (output directly on pin).
 * 
 * @param RotationDir    ->  ROT_CW for clocwWise direction, ROT_CC for counter-clockwise direction.
 *
 * N.B. Real rotation direction depends from motor board hardware, if your direction does not match, use EnableDirSwapMode(true) to correct it.
 */
void DStepper::SetDir(DRotationDir RotationDir)
{
    WritePin(DirPin,RotationDir ^ SwappedDir);
}

/**
 * @brief Set rotation direction clockwise (output directly on pin).
 */
void DStepper::SetDirCW(void)
{
    SetDir(ROT_CW);
}

/**
 * @brief Set rotation direction counter-clockwise (output directly on pin).
 */
void DStepper::SetDirCC(void)
{
    SetDir(ROT_CC);
}

/**
 * @brief Revert current rotation direction (output directly on pin).
 */
void DStepper::RevertDir(void)
{
    WritePin(DirPin,ReadPin(DirPin) ^1 );
}
