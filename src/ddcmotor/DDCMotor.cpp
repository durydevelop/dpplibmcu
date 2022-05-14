#include <dpplib-gpio.h>
#include "DDCMotor.h"

// Defaults
#define DEFAULT_STEP_VALUE  10      //! Default vel step inc/dec value

// Ranges limits
#define DEFAULT_CAL_LIMIT MAX_PWM

/**
 * @brief Construct a new DDCMotor::DDCMotor object
 * 
 */
DDCMotor::DDCMotor()
{
    PwmPin=0;
	DirPwmPin=0;
	ControlMode=DIR_PWM;
	Attached=false;
    Running=false;
}

/**
 * @brief Construct a new DDCMotor::DDCMotor object
 * 
 * @param PwmPin    ->  In DIR_PWM mode pin is xENABLE(PWM).
 *                      In DIR_DIR mode pin is PWM pin.
 * @param DirPwmPin ->  In DIR_PWM mode pin is xPHASE(DIR).
 *                      In DIR_DIR mode pin is PWM pin.
 * @param Mode      ->  The DIR_PWM mode only 2 pin are used in pwm mode (use it with board like "Pololu DRV8825 Dual Motor Driver Kit").
 *                      The DIR_DIR mode all 4 pins are used in pwm mode.
 *                      
 */
DDCMotor::DDCMotor(unsigned short int PinPwm, unsigned short int PinDirPwm, DControlMode Mode)
{
    attach(PinPwm,PinDirPwm,Mode);
}

/**
 * @brief Destroy the DDCMotor::DDCMotor object
 */
DDCMotor::~DDCMotor()
{
    detach();
}

/**
 * @brief Initialize Hardware.
 * 
 * @param PwmPin    ->  In PHASE_ENABLE mode pin is xENABLE(PWM).
 *                      In IN_IN mode pin is PWM pin.
 * @param DirPwmPin ->  In PHASE_ENABLE mode pin is xPHASE(DIR).
 *                      In IN_IN mode pin is PWM pin.
 * @param Mode      ->  The IN_IN mode all 4 pins are used in pwm mode.
 *                      The PHASE_ENABLE mode only 2 pin are used pwm mode (use it with board like "Pololu DRV8825 Dual Motor Driver Kit")
 */
void DDCMotor::attach(unsigned short int PinPwm, unsigned short int PinDirPwm, DControlMode Mode)
{
    PwmPin=PinPwm;
    DirPwmPin=PinDirPwm;
    ControlMode=Mode;

    Attached=InitPin(PinPwm,OUTPUT);
    Attached=InitPin(DirPwmPin,OUTPUT);

    // Defaults
	ResetCalLimit();
	CurrStepValue=DEFAULT_STEP_VALUE;
	CurrVel=0;
    SwappedDir=0;
    Running=false;

	Stop();
}

/**
 * @brief Release hardware.
 */
void DDCMotor::detach(void)
{
    if (Attached) {
        Stop();
        Attached=false;
        #ifdef PIGPIO_VERSION
            gpioTerminate();
        #endif
    }
}

/**
 * @return true if hardware pins are initialized, otherwise false;
 */
bool DDCMotor::attached(void)
{
    return(Attached);
}

/**
 * @brief Reset all PWM calibration values to default.
 */
void DDCMotor::ResetCalLimit(void)
{
	ResetCalLimitFw();
	ResetCalLimitRev();
}

/**
 * @brief Reset forward PWM calibration value to default.
 */
void DDCMotor::ResetCalLimitFw(void)
{
    PwmLimitFw=DEFAULT_CAL_LIMIT;
    if (CurrVel != 0) {
        // Motor is running, update vel
        Set(CurrVel);
	}
}

/**
 * @brief Reset reverse PWM calibration value to default.
 */
void DDCMotor::ResetCalLimitRev(void)
{
    PwmLimitRev=DEFAULT_CAL_LIMIT;
    if (Running) {
        // Motor is running, update vel
        Set(CurrVel);
	}
}

/**
* @return current motor vel (from -100 to 100)
**/
short int DDCMotor::GetVel(void)
{
	return(CurrVel);
}

/**
* @return current forward PWM calibration value
**/
unsigned short int DDCMotor::GetCalLimitFw(void)
{
	return(PwmLimitFw);
}

/**
* @return current reverse PWM calibration value
**/
unsigned short int DDCMotor::GetCalLimitRev(void)
{
	return(PwmLimitRev);
}

/**
 * @brief Set PWM calibration for forward direction.
 * PwmLimitFw value will be used as maximun value for motor in forward direction (vel set to 100).
 * 
 * @param NewPwmLimitFw PWM value from MIN_CAL_LIMIT to MAX_PWM.
 */
void DDCMotor::SetCalLimitFw(unsigned short int NewPwmLimitFw)
{
    if (NewPwmLimitFw > MAX_PWM) {
        PwmLimitFw=MAX_PWM;
    }
    else if (NewPwmLimitFw < MIN_CAL_PWM) {
        PwmLimitFw=MIN_CAL_PWM;
    }
    else {
        PwmLimitFw=NewPwmLimitFw;
    }

    if (Running) {
        // Motor is running, update vel
        Set(CurrVel);
	}
}

/**
 * @brief Set PWM calibration for backward direction.
 * PwmLimitFw value will be used as maximun value for motor in forward direction (vel set to 100).
 * 
 * @param NewPwmLimitRev PWM value from MIN_CAL_LIMIT to MAX_PWM.
 */
void DDCMotor::SetCalLimitRev(unsigned short int NewPwmLimitRev)
{
    if (NewPwmLimitRev > MAX_PWM) {
        PwmLimitRev=MAX_PWM;
    }
    else if (NewPwmLimitRev < MIN_CAL_PWM) {
        PwmLimitRev=MIN_CAL_PWM;
    }
    else {
        PwmLimitRev=NewPwmLimitRev;
    }

    if (Running) {
        // Motor is running, update vel
        Set(CurrVel);
	}
}

/**
 * @brief Runs motor in clockwise direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrVel is set.
 * 
 * @param Speed Speed value from 1 to 100. If Vel is 0 CurrVel is used.
 */
void DDCMotor::CW(unsigned short int Speed)
{
    if (Speed == 0) {
        Set(abs(CurrVel));
    }
    else {
	    Set(Speed);
    }
}

/**
 * @brief Runs motor in counter clockwise direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrVel is set.
 * 
 * @param Speed Speed value from 1 to 100. If Vel is 0 CurrVel is used.
 */
void DDCMotor::CC(unsigned short int Speed)
{
    if (Speed == 0) {
        Set(-abs(CurrVel));
    }
    else {
	    Set(-Speed);
    }
}

/**
 * @brief Stop motor (set vel to 0).
 * 
 */
void DDCMotor::Stop(void)
{
	Set(0);
}

/**
 * @brief Set step value used by functions for direct increment - decrecrement speed.
 * 
 * @param Value ->  Valid step values: from 1 to MAX_VEL (a zero value is set to 1)
 */
void DDCMotor::SetIncValue(unsigned short int Value)
{
    CurrStepValue=Value;
    if (CurrStepValue == 0) {
        CurrStepValue=1;
    }
    if (CurrStepValue > MAX_VEL) {
        CurrStepValue=MAX_VEL;
    }
}

/**
 * @brief Calculate real pwm value corresponding to CurrVel
 * 
 * @return Current PWM value from 0 to PwmLimitFw (if running forward) or 0 to PwmLimitRev (if running reverse)
 */
unsigned short int DDCMotor::GetPwm(void)
{
    if (CurrVel > 0) {
        return(map(abs(CurrVel),0,MAX_VEL,0,PwmLimitFw));
    }
    else if (CurrVel < 0) {
        return(map(abs(CurrVel),0,MAX_VEL,0,PwmLimitRev));
    }
    else {
        return(0);
    }
}

/**
 * @brief 
 * 
 * N.B. Real rotation direction depends from motor connection, if your direction does not match, and you can not swap motor wires connection, use EnableDirSwapMode(true) to correct it.
 * 
 * @return ROT_CW for clocwWise direction, ROT_CC for counter-clockwise direction.
 */
DDCMotor::DRotationDir DDCMotor::GetDir(void)
{
    return(CurrVel > 0 ? ROT_CW : CurrVel < 0 ? ROT_CC : ROT_NONE);
}

/**
 * @brief Swap level on PinDir when set rotation direction.
 * Use it with argument set to true if your motor rotate in wrong direction.
 * 
 * @param Enabled 
 */
void DDCMotor::SwappedDirMode(bool Enabled)
{
    if (Enabled) {
        SwappedDir=1;
    }
    else {
        SwappedDir=0;
    }
}

/**
 * @brief Run motor.
 * Vel is value from -100 to 100 where positive values runs motor in forward direction and negatives ones runs motor in backward direction.
 * A value of 0 stop motor.
 * 
 * @param Vel    ->  Absolute velocity value for motor: from -100 to 100, 0 stops the motor.
 */
void DDCMotor::Set(short Speed)
{
    // Store vel
    CurrVel=Speed ^ SwappedDir;
    
    // Correct over value
    //if (CurrVel > 0 && CurrVel > MAX_ABS_VEL) {
    if (CurrVel > 0 && CurrVel > MAX_VEL) {
        CurrVel=MAX_VEL;
    }
    else if (CurrVel < 0 && CurrVel < -MAX_VEL) {
        CurrVel=-MAX_VEL;
    }

    // Output
    if (CurrVel > 0) {
        // Calculate PWM value remapped from 0 to CalMaxPwmFw
        unsigned short int PwmValue=map(abs(CurrVel),0,MAX_VEL,0,PwmLimitFw);
        // Forward
        if (ControlMode == PWM_PWM) {
            PwmWrite(PwmPin,0);
            PwmWrite(DirPwmPin,PwmValue);
		}
		else {
		    PwmWrite(PwmPin,PwmValue);
            WritePin(DirPwmPin,LOW);
		}
        Running=true;
	}
	else if (CurrVel < 0) {
        // Calculate PWM value remapped from 0 to CalMaxPwmRev
        unsigned short int PwmValue=map(abs(CurrVel),0,MAX_VEL,0,PwmLimitRev);
        // Reverse
        if (ControlMode == PWM_PWM) {
            PwmWrite(PwmPin,PwmValue);
            PwmWrite(DirPwmPin,0);
		}
		else {
		    PwmWrite(PwmPin,PwmValue);
            WritePin(DirPwmPin,HIGH);
		}
        Running=true;
	}
	else {
        // Stop
		if (ControlMode == PWM_PWM) {
            PwmWrite(PwmPin,0);
            PwmWrite(DirPwmPin,0);

        }
        else {
		    PwmWrite(PwmPin,0);
            WritePin(DirPwmPin,LOW);
        }
        Running=false;
	}
}

/**
 * @brief Brake motor.
 * Output signals to pins for blocking motor.
 */
void DDCMotor::Brake(void)
{
    if (ControlMode == PWM_PWM) {
        PwmWrite(PwmPin,MAX_PWM);
        PwmWrite(DirPwmPin,MAX_PWM);
    }
    else {
        // DIR_PWM
        PwmWrite(PwmPin,0);
        WritePin(DirPwmPin,LOW);
    }
    CurrVel=0;
    Running=false;
}

/**
 * @return true if motor is running, otherwise false.
 */
bool DDCMotor::IsRunning(void)
{
    return(Running);
}

/**
 * @brief Get pin number that is only used as PWM.
 * 
 * @return PWM pin number.
 */
unsigned short int DDCMotor::GetPinPwm(void)
{
    return(PwmPin);
}

/**
 * @brief Get pin number that is use as DIR (in PHASE_ENABLED mode) or PWM (in IN_IN mode).
 * 
 * @return pin b of Motor 1
 */
unsigned short int DDCMotor::GetPinDirPwm(void)
{
    return(DirPwmPin);
}
