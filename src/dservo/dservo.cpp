#include "dservo.h"
#include <iostream>

#define MAX_STEP_DEGREES 90
#define MAX_STEP_US 1000
#define DEFAULT_STEP_US 10

/**
 * @brief Construct a new DServo::DServo object and attach to pin.
 * @param pin    ->  Pwm output pin
 */
DServo::DServo(int gpioPin, DGpioHandle gpioHandle)
{
    pin=gpioPin;
    dPwmOut=nullptr;
    handle=gpioHandle;
    setDefaultValues(false);
}

DServo::~DServo()
{
    detach();
}

/**
 * N.B. Only values are chaged (no servo re-positioning)
 */
void DServo::setDefaultValues(bool outServo)
{
    minPulseUs=DEFAULT_MIN_PULSE_US;
    maxPulseUs=DEFAULT_MAX_PULSE_US;
    resetStepValue();
    resetHomeValue();
    currDegrees=90;
    currPulseUs=degreesToUs(currDegrees);

    if (outServo) {
        servoOut();
    }
}

/**
* @param Pin    ->  Servo attached pin nr
**/
void DServo::begin(void)
{
    // Create software pwm for clock pin
    if (dPwmOut) {
        delete dPwmOut;
    }

    dPwmOut=new DPwmOut(pin,handle);
    dPwmOut->begin(50);
    dPwmOut->setMicros(1500);
    dPwmOut->on();
}

/**
* Reset servo pin to output mode and set it to LOW
**/
void DServo::detach(void)
{
    if (dPwmOut) {
        currPulseUs=0;
        servoOut();
        delete dPwmOut;
        dPwmOut=nullptr;
    }
}

/**
* @return @Attached value
**/
bool DServo::attached(void)
{
    return(dPwmOut->isReady());
}

/**
* @param Degrees-> Degrees value from 0 to 180
*/
void DServo::write(short int degrees)
{
    // Clamp Degrees
    if (degrees < 0) degrees=0;
    if (degrees > 180) degrees=180;
    currDegrees=degrees;
    // map Degrees into real pwm range for this servo
    currPulseUs=degreesToUs(currDegrees);
    // Output it
    servoOut();
}

/**
 * @param Us -> Micro seconds value. It can be a value from 500 to 2500 but the reaction depends on servo motor.
 */
void DServo::writeMicroseconds(int us)
{
    if (us != 0) {
        // Clamp Us
        if (us < 500) us=500;
        else if (us > 2500) us=2500;
    }
    else {
        if (currPulseUs == 0) us=(maxPulseUs-minPulseUs)/2;
    }

    currPulseUs=us;
    // Calculate currDegrees
    currDegrees=usToDegrees(currPulseUs);
    // Output it
    servoOut();
}

//! @return current servo position in degrees from 0 to 180
short int DServo::read(void)
{
    return(currDegrees);
}

/**
* @param Pos -> position from 0.0 to 1.0 where 0.5 is centre position
*/
void DServo::writeRange(float Pos)
{
    // Clamp Pos
    if (Pos > 1.0) Pos=1.0;
    // Calculate currDegrees
    currDegrees=Pos*180;
    // map Degrees into real pwm range for this servo
    currPulseUs=degreesToUs(currDegrees);
    // Output it
    servoOut();
}

/**
* @param Degrees-> Degrees value from 0 to 180
*/
void DServo::writeDegrees(short int degrees)
{
    write(degrees);
}

void DServo::fullCCW(void)
{
    write(180);
}

void DServo::fullCW(void)
{
    write(0);
}

void DServo::center(void)
{
    write(90);
}

/**
 * @brief Home position is an arbitrary value that can be set using SetHomePositionUs() or SetHomePositionDegree()
 */
void DServo::home(void)
{
    writeMicroseconds(currHomeUs);
}

void DServo::stepCCWDegrees(short int DegreeStep)
{
    write(currDegrees+DegreeStep);
}

void DServo::stepCWDegrees(short int DegreeStep)
{
    write(currDegrees-DegreeStep);
}

void DServo::stepCCW(void)
{
    stepCCWUs(currStepUs);
}

void DServo::stepCW(void)
{
    stepCWUs(currStepUs);
}

void DServo::stepCCWUs(int usStep)
{
    writeMicroseconds(currPulseUs+usStep);
}

void DServo::stepCWUs(int usStep)
{
    writeMicroseconds(currPulseUs-usStep);
}

void DServo::setMinPulseUs(int us)
{
    minPulseUs=us;
    if (currPulseUs < minPulseUs) {
		currPulseUs=minPulseUs;
    }
    writeMicroseconds(currPulseUs);
}

void DServo::setMaxPulseUs(int us)
{
    maxPulseUs=us;
    if (currPulseUs > maxPulseUs) {
		currPulseUs=maxPulseUs;
    }
    writeMicroseconds(currPulseUs);
}

int DServo::getMaxPulseUs(void)
{
    return(maxPulseUs);
}

int DServo::getMinPulseUs(void)
{
    return(minPulseUs);
}

short int DServo::getCurrDegrees(void)
{
    return(currDegrees);
}

int DServo::getCurrPulseUs(void)
{
    return(currPulseUs);
}

/**
* @param Value    ->  step value from 1 to DDCMW_MAX_VEL
**/
void DServo::setStepDegrees(short int degrees)
{
    currStepDegrees=degrees;
    if (currStepDegrees < 1) {
        currStepDegrees=1;
    }
    if (currStepDegrees > MAX_STEP_DEGREES) {
        currStepDegrees=MAX_STEP_DEGREES;
    }
    // Update also Us
    currStepUs=degreesToUs(currStepDegrees);
}

short int DServo::getStepDegrees(void)
{
    return(currStepDegrees);
}

/**
* @param Value    ->  step value from 1 to DDCMW_MAX_VEL
**/
void DServo::setStepUs(int us)
{
    currStepUs=us;
    if (currStepUs < 1) {
        currStepUs=1;
    }
    if (currStepUs > MAX_STEP_US) {
        currStepUs=MAX_STEP_US;
    }
    // Update also Degree
    currStepDegrees=usToDegrees(currStepUs);
}

int DServo::getStepUs(void)
{
    return(currStepUs);
}

void DServo::setHomeDegrees(short int degrees)
{
    currHomeUs=degreesToUs(degrees);
}

short int DServo::getHomeDegrees(void)
{
    return(usToDegrees(currHomeUs));
}

void DServo::setHomeUs(int us)
{
    currHomeUs=us;
}

int DServo::getHomeUs(void)
{
    return(currHomeUs);
}

/**
 * Servo will be re-positioned
 * 
 * @return int New current mininum pulse-width in us.
 */
int DServo::resetMinPulseValue(void)
{
	minPulseUs=DEFAULT_MIN_PULSE_US;
	if (currPulseUs < minPulseUs) {
		currPulseUs=minPulseUs;
    }
    write(currDegrees);

    return(minPulseUs);
}

/**
 * Servo will be re-positioned
 * 
 * @return int New current maxinum pulse-width in us.
 */
int DServo::resetMaxPulseValue(void)
{
	maxPulseUs=DEFAULT_MAX_PULSE_US;
	if (currPulseUs > maxPulseUs) {
		currPulseUs=maxPulseUs;
    }
    write(currDegrees);

    return(maxPulseUs);
}

/**
 * @return int New current step pulse-width in us.
 */
int DServo::resetStepValue(void)
{
    currStepUs=DEFAULT_STEP_US;
    currStepDegrees=usToDegrees(currStepUs);
    return(currStepUs);
}

/**
 * Servo will not be re-positioned
 * 
 * @return int New current home pulse-width in us.
 */
int DServo::resetHomeValue(void)
{
    currHomeUs=(maxPulseUs-minPulseUs)/2;
    return(currHomeUs);
}

void DServo::servoOut(void)
{
    if (dPwmOut) {
        dPwmOut->setMicros(currPulseUs);
    }
}

int DServo::degreesToUs(short int degrees)
{
    return(mapValue(currDegrees,0,180,minPulseUs,maxPulseUs));
}

short int DServo::usToDegrees(int us)
{
    return(mapValue(us,minPulseUs,maxPulseUs,0,180));
}

short int DServo::getPin(void)
{
    return(dPwmOut->getPin());
}

std::string DServo::getLastError(void)
{
    if (dPwmOut) {
        return dPwmOut->getLastError();
    }
    else {
        return getErrorCode(DERR_CLASS_NOT_BEGUN);
    }
}