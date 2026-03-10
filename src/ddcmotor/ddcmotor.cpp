#include "ddcmotor.h"
#include <dutils>

// Defaults
#define DEFAULT_STEP_VALUE  10      //! Default vel step inc/dec value
#define PWM_FREQ 480

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
    PwmOut1=new DPwmOut(PinPwm);
    PwmOut2=new DPwmOut(PinDirPwm);
	ControlMode=Mode;
    Running=false;
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
 * Must be called before use this class.
 */
void DDCMotor::attach(void)
{
    // **** Pwm Out 1 ****
/*
    if (PwmOut1) {
        // Already istantiated
        if (PinPwm != PwmOut1->getPin()) {
            // on different pin
            delete PwmOut1;
        }
    }
*/
    // Pwm settings
    // 480 Hz 50% duty
//    PwmOut1=new DPwmOut(PinPwm);
    PwmOut1->begin(PWM_FREQ,50,true);
    // Max Value used for WriteMicros()
    maxPwmValue=PwmOut1->getPeriodUs();
    // Min value for calibration
    minCalPwmValue=maxPwmValue/3;
/*
    // **** Pwm Out 2 / Dir Pin****
    if (PwmOut2) {
        // Already istantiated
        if (PinDirPwm != PwmOut2->getPin()) {
            // on different pin
            delete PwmOut2;
        }
    }
    PwmOut2=new DPwmOut(PinDirPwm);
*/
    // Pwm settings
    // 480 Hz 50% duty
    PwmOut2->begin(PWM_FREQ,50,true);

    // Defaults
	ResetPwmLimit();
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
    if (attached()) {
        Stop();
//        Attached=false;
        PwmOut1->release();
        PwmOut2->release();
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
    return(PwmOut1->isReady() & PwmOut2->isReady());
}

/**
 * @brief Reset all PWM calibration values to default.
 */
void DDCMotor::ResetPwmLimit(void)
{
	ResetPwmLimitFw();
	ResetPwmLimitRev();
}

/**
 * @brief Reset forward PWM calibration value to default (maxPwmValue).
 */
void DDCMotor::ResetPwmLimitFw(void)
{
    PwmLimitFw=maxPwmValue;
    if (CurrVel != 0) {
        // Motor is running, update vel
        SetVel(CurrVel);
	}
}
/**
 * @brief Reset reverse PWM calibration value to default (maxPwmValue).
 */
void DDCMotor::ResetPwmLimitRev(void)
{
    PwmLimitRev=maxPwmValue;
    if (Running) {
        // Motor is running, update vel
        SetVel(CurrVel);
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
unsigned short int DDCMotor::GetPwmLimitFw(void)
{
	return(PwmLimitFw);
}

/**
* @return current reverse PWM calibration value
**/
unsigned short int DDCMotor::GetPwmLimitRev(void)
{
	return(PwmLimitRev);
}

/**
* @return current max PWM value
**/
unsigned short int DDCMotor::GetMaxPwmValue(void)
{
	return(maxPwmValue);
}
/*
short int DDCMotor::GetVelLimitFw(void)
{
    return(PwmToVel(PwmLimitFw));
}

short int DDCMotor::GetVelLimitRev(void)
{
    return(PwmToVel(PwmLimitRev));
}
*/
/**
 * @brief Set PWM calibration for forward direction.
 * PwmLimitFw value will be used as maximun value for motor in forward direction (when is vel set to 100).
 * 
 * @param NewPwmLimitFw PWM value from minCalPwmValue to maxPwmValue.
 */
void DDCMotor::SetPwmLimitFw(unsigned short int NewPwmLimitFw)
{
    if (NewPwmLimitFw > maxPwmValue) {
        PwmLimitFw=maxPwmValue;
    }
    else if (NewPwmLimitFw < minCalPwmValue) {
        PwmLimitFw=minCalPwmValue;
    }
    else {
        PwmLimitFw=NewPwmLimitFw;
    }

    if (Running) {
        // Motor is running, update vel
        SetVel(CurrVel);
	}
}

/**
 * @brief Set PWM calibration for backward direction.
 * PwmLimitFw value will be used as maximun value for motor in forward direction (vel set to 100).
 * 
 * @param NewPwmLimitRev PWM value from minCalPwmValue to maxPwmValue.
 */
void DDCMotor::SetPwmLimitRev(unsigned short int NewPwmLimitRev)
{
    if (NewPwmLimitRev > maxPwmValue) {
        PwmLimitRev=maxPwmValue;
    }
    else if (NewPwmLimitRev < minCalPwmValue) {
        PwmLimitRev=minCalPwmValue;
    }
    else {
        PwmLimitRev=NewPwmLimitRev;
    }

    if (Running) {
        // Motor is running, update vel
        SetVel(CurrVel);
	}
}

/**
 * @brief Runs motor in clockwise direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrVel is set.
 * 
 * @param Vel Speed value from 1 to 100. If Vel is 0 CurrVel is used.
 */
void DDCMotor::CW(unsigned short int Vel)
{
    if (Vel == 0) {
        SetVel(abs(CurrVel));
    }
    else {
	    SetVel(Vel);
    }
}

/**
 * @brief Runs motor in counter clockwise direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrVel is set.
 * 
 * @param Vel Speed value from 1 to 100. If Vel is 0 CurrVel is used.
 */
void DDCMotor::CC(unsigned short int Vel)
{
    if (Vel == 0) {
        SetVel(-abs(CurrVel));
    }
    else {
	    SetVel(-Vel);
    }
}

/**
 * @brief Stop motor (set vel to 0).
 * 
 */
void DDCMotor::Stop(void)
{
	SetVel(0);
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
    return VelToPwm(CurrVel);
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
 * A value of 0 stops motor.
 * 
 * @param Vel    ->  Absolute velocity value for motor: from -100 to 100, 0 stops the motor.
 */
void DDCMotor::SetVel(int Speed)
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

    // Calculate remapped PWM value
    unsigned short int PwmValue=VelToPwm(CurrVel);

    // Output
    if (CurrVel > 0) {
        // Forward
        if (ControlMode == PWM_PWM) {
            PwmOut1->setMicros(0);
            PwmOut2->setMicros(PwmValue);
		}
		else {
            PwmOut2->high();
            PwmOut1->setMicros(PwmValue);
            
		}
        Running=true;
	}
	else if (CurrVel < 0) {
        // Reverse
        if (ControlMode == PWM_PWM) {
            PwmOut1->setMicros(PwmValue);
            PwmOut2->setMicros(0);
		}
		else {
            PwmOut2->low();
		    PwmOut1->setMicros(PwmValue);
            
		}
        Running=true;
	}
	else {
        // Stop
		if (ControlMode == PWM_PWM) {
            PwmOut1->setMicros(0);
            PwmOut2->setMicros(0);

        }
        else {
		    PwmOut1->low();
            PwmOut2->low();
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
        PwmOut1->setMicros(maxPwmValue);
        PwmOut2->setMicros(maxPwmValue);
    }
    else {
        // DIR_PWM
        PwmOut1->low();
        PwmOut2->low();
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
    return(PwmOut1->getPin());
}

/**
 * @brief Get pin number that is use as DIR (in PHASE_ENABLED mode) or PWM (in IN_IN mode).
 * 
 * @return DIR / PWN pin number.
 */
unsigned short int DDCMotor::GetPinDirPwm(void)
{
    return(PwmOut2->getPin());
}

/**
 * @brief Convert velocity value to PWM value.
 * 
 * @param Vel 
 * @return unsigned short int 
 */
unsigned short int DDCMotor::VelToPwm(short int Vel)
{
    if (Vel > 0) {
        return(mapValue(abs(Vel),0,MAX_VEL,0,PwmLimitFw));
    }
    else if (Vel < 0) {
        return(mapValue(abs(Vel),0,MAX_VEL,0,PwmLimitRev));
    }
    else {
        return(0);
    }
}

/**
 * @brief Convert PWM value to velocity value.
 * 
 * @param Pwm 
 * @return short int 
 */
/*
short int DDCMotor::PwmToVel(unsigned short int Pwm)
{
    if (Pwm > 0) {
        return(mapValue(Pwm,0,PwmLimitFw,0,MAX_VEL));
    }
    else if (Pwm < 0) {
        return(mapValue(Pwm,0,PwmLimitRev,0,-MAX_VEL));
    }
    else {
        return(0);
    }
}
*/