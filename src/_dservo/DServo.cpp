#include <dpplib-gpio>
#include "DServo.h"

/**
 * TODO:
 *      _Test ARDUINO
 *      _mbed
 */

/**
#Raspberry-pi HARWARE PWM:
Hardware pwm let control servo at 50 Hz within 100 steps from 100 to 200
_CLOCK=192, RANGE=2000
_19.2 MHz / (CLOCK * RANGE) = 50Hz (19.2MHz base frequency of the PWM clock)
_50Hz has a period of 20ms
_RANGE is 2000 pulse units so, 20ms/2000=10us is width of single pulse unit
_Servo works from 1.0ms to 2.0ms where 1.5 ms is centre position
_100 pulse units * 10us=1ms fully counter-clockwise
_200 pulse units * 10us=2ms fully clockwise

SOFTWARE PWM:
Software pwm let control servo at 50 Hz within 20 steps from 10 to 20
_Pulse unit is 100 us long.
_softPwmCreate(Pin,0,200) create a period of 200 pulse unit so, 100us * 200 = 20ms (50Hz Frequency)
_softPwmWrite(1,10) out 10 steps of 100 us so, 10 pulse units * 100us = 1ms fully counter-clockwise
_softPwmWrite(1,20) out 20 steps of 100 us so, 20 pulse units * 100us = 2ms fully clockwise

**/

//#define map(x,in_min,in_max,out_min,out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min

#define MAX_STEP_DEGREES 90
#define MAX_STEP_US 1000
#define DEFAULT_STEP_US 10

DServo::DServo()
{
    dPwmOut=nullptr;
    SetDefaultValues(false);
}

/**
* If You want to directly set pin servo declaration
* @param Pin    ->  Pwm output pin
*/
DServo::DServo(short int Pin)
{
    dPwmOut=nullptr;
    SetDefaultValues(false);
    attach(Pin);
}

DServo::~DServo()
{
    detach();
}

/**
 * N.B. Only values are chaged (no servo re-positioning)
 */
void DServo::SetDefaultValues(bool OutServo)
{
    MinPulseUs=DEFAULT_MIN_PULSE_US;
    MaxPulseUs=DEFAULT_MAX_PULSE_US;
    ResetStepValue();
    ResetHomeValue();
    CurrDegrees=90;
    CurrPulseUs=DegreesToUs(CurrDegrees);

    if (OutServo) {
        ServoOut();
    }
}

/**
* @param Pin    ->  Servo attached pin nr
**/
void DServo::attach(int Pin)
{
    // Create software pwm for clock pin
    if (dPwmOut) {
        delete dPwmOut;
    }

    dPwmOut=new DPwmOut(Pin);
    dPwmOut->begin();
    dPwmOut->SetFreq(50);
    dPwmOut->SetMicros(1500);
    dPwmOut->On();
}

/**
* Reset servo pin to output mode and set it to LOW
**/
void DServo::detach(void)
{
    if (dPwmOut) {
        CurrPulseUs=0;
        ServoOut();
        delete dPwmOut;
        dPwmOut=nullptr;
        ShutdownGpio();
    }
}

/**
* @return @Attached value
**/
bool DServo::attached(void)
{
    return(dPwmOut->IsReady());
}

/**
* @param Degrees-> Degrees value from 0 to 180
*/
void DServo::write(short int Degrees)
{
    // Clamp Degrees
    if (Degrees < 0) Degrees=0;
    if (Degrees > 180) Degrees=180;
    CurrDegrees=Degrees;
    // map Degrees into real pwm range for this servo
    CurrPulseUs=DegreesToUs(CurrDegrees);
    // Output it
    ServoOut();
}

/**
 * @param Us -> Micro seconds value. It can be a value from 500 to 2500 but the reaction depends on servo motor.
 */
void DServo::writeMicroseconds(int Us)
{
    if (Us != 0) {
        // Clamp Us
        if (Us < 500) Us=500;
        else if (Us > 2500) Us=2500;
    }
    else {
        if (CurrPulseUs == 0) Us=(MaxPulseUs-MinPulseUs)/2;
    }

    CurrPulseUs=Us;
    // Calculate CurrDegrees
    CurrDegrees=UsToDegrees(CurrPulseUs);
    // Output it
    ServoOut();
}

/**
 * @param Us -> Micro seconds value. It can be a value from 500 to 2500 but the reaction depends on servo motor.
 */
void DServo::WriteMicroseconds(int Us)
{
    writeMicroseconds(Us);
}

//! @return current servo position in degrees from 0 to 180
short int DServo::read(void)
{
    return(CurrDegrees);
}

/**
* @param Pos -> position from 0.0 to 1.0 where 0.5 is centre position
*/
void DServo::WriteRange(float Pos)
{
    // Clamp Pos
    if (Pos > 1.0) Pos=1.0;
    // Calculate CurrDegrees
    CurrDegrees=Pos*180;
    // map Degrees into real pwm range for this servo
    CurrPulseUs=DegreesToUs(CurrDegrees);
    // Output it
    ServoOut();
}

/**
* @param Degrees-> Degrees value from 0 to 180
*/
void DServo::WriteDegrees(short int Degrees)
{
    write(Degrees);
}

void DServo::FullCCW(void)
{
    write(180);
}

void DServo::FullCW(void)
{
    write(0);
}

void DServo::Center(void)
{
    write(90);
}

/**
 * @brief Home position is an arbitrary value that can be set using SetHomePositionUs() or SetHomePositionDegree()
 */
void DServo::Home(void)
{
    writeMicroseconds(CurrHomeUs);
}

void DServo::StepCCWDegrees(short int DegreeStep)
{
    write(CurrDegrees+DegreeStep);
}

void DServo::StepCWDegrees(short int DegreeStep)
{
    write(CurrDegrees-DegreeStep);
}

void DServo::StepCCW(void)
{
    StepCCWUs(CurrStepUs);
}

void DServo::StepCW(void)
{
    StepCWUs(CurrStepUs);
}

void DServo::StepCCWUs(int UsStep)
{
    writeMicroseconds(CurrPulseUs+UsStep);
}

void DServo::StepCWUs(int UsStep)
{
    writeMicroseconds(CurrPulseUs-UsStep);
}

void DServo::SetMinPulseUs(int Us)
{
    MinPulseUs=Us;
    if (CurrPulseUs < MinPulseUs) {
		CurrPulseUs=MinPulseUs;
    }
    writeMicroseconds(CurrPulseUs);
}

void DServo::SetMaxPulseUs(int Us)
{
    MaxPulseUs=Us;
    if (CurrPulseUs > MaxPulseUs) {
		CurrPulseUs=MaxPulseUs;
    }
    writeMicroseconds(CurrPulseUs);
}

int DServo::GetMaxPulseUs(void)
{
    return(MaxPulseUs);
}

int DServo::GetMinPulseUs(void)
{
    return(MinPulseUs);
}

short int DServo::GetCurrDegrees(void)
{
    return(CurrDegrees);
}

int DServo::GetCurrPulseUs(void)
{
    return(CurrPulseUs);
}

/**
* @param Value    ->  step value from 1 to DDCMW_MAX_VEL
**/
void DServo::SetStepDegrees(short int Degrees)
{
    CurrStepDegrees=Degrees;
    if (CurrStepDegrees < 1) {
        CurrStepDegrees=1;
    }
    if (CurrStepDegrees > MAX_STEP_DEGREES) {
        CurrStepDegrees=MAX_STEP_DEGREES;
    }
    // Update also Us
    CurrStepUs=DegreesToUs(CurrStepDegrees);
}

short int DServo::GetStepDegrees(void)
{
    return(CurrStepDegrees);
}

/**
* @param Value    ->  step value from 1 to DDCMW_MAX_VEL
**/
void DServo::SetStepUs(int Us)
{
    CurrStepUs=Us;
    if (CurrStepUs < 1) {
        CurrStepUs=1;
    }
    if (CurrStepUs > MAX_STEP_US) {
        CurrStepUs=MAX_STEP_US;
    }
    // Update also Degree
    CurrStepDegrees=UsToDegrees(CurrStepUs);
}

int DServo::GetStepUs(void)
{
    return(CurrStepUs);
}

void DServo::SetHomeDegrees(short int Degrees)
{
    CurrHomeUs=DegreesToUs(Degrees);
}

short int DServo::GetHomeDegrees(void)
{
    return(UsToDegrees(CurrHomeUs));
}

void DServo::SetHomeUs(int Us)
{
    CurrHomeUs=Us;
}

int DServo::GetHomeUs(void)
{
    return(CurrHomeUs);
}

/**
 * Servo will be re-positioned
 * 
 * @return int New current mininum pulse-width in us.
 */
int DServo::ResetMinPulseValue(void)
{
	MinPulseUs=DEFAULT_MIN_PULSE_US;
	if (CurrPulseUs < MinPulseUs) {
		CurrPulseUs=MinPulseUs;
    }
    write(CurrDegrees);

    return(MinPulseUs);
}

/**
 * Servo will be re-positioned
 * 
 * @return int New current maxinum pulse-width in us.
 */
int DServo::ResetMaxPulseValue(void)
{
	MaxPulseUs=DEFAULT_MAX_PULSE_US;
	if (CurrPulseUs > MaxPulseUs) {
		CurrPulseUs=MaxPulseUs;
    }
    write(CurrDegrees);

    return(MaxPulseUs);
}

/**
 * @return int New current step pulse-width in us.
 */
int DServo::ResetStepValue(void)
{
    CurrStepUs=DEFAULT_STEP_US;
    CurrStepDegrees=UsToDegrees(CurrStepUs);
    return(CurrStepUs);
}

/**
 * Servo will not be re-positioned
 * 
 * @return int New current home pulse-width in us.
 */
int DServo::ResetHomeValue(void)
{
    CurrHomeUs=(MaxPulseUs-MinPulseUs)/2;
    return(CurrHomeUs);
}

void DServo::ServoOut(void)
{
    if (dPwmOut) {
        #ifdef ARDUINO
            dPwmOut->SetMicros(CurrPulseUs);
        #else
            #ifdef PIGPIO_VERSION
                gpioServo(ServoPin,CurrPulseUs);
            #else
                // TODO
            #endif
            
        #endif
    }

}

int DServo::DegreesToUs(short int Degrees)
{
    return(map(CurrDegrees,0,180,MinPulseUs,MaxPulseUs));
}

short int DServo::UsToDegrees(int Us)
{
    return(map(Us,MinPulseUs,MaxPulseUs,0,180));
}

short int DServo::GetServoPin(void)
{
    return(dPwmOut->GetPin());
}
