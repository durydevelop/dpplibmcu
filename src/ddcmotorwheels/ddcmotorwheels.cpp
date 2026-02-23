#include "DDCMotorWheels.h"
#ifdef ARDUINO
	// Arduino inlcudes
    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "wiring.h"
        #include "WProgram.h"
    #endif
#else
    #include <cstdlib>
#endif

#define DEFAULT_INCREMENTAL_VALUE  10      //! Default step for increment - decrement vel

/**
 * @brief Construct a new DDCMotorWheels::DDCMotorWheels object.
 * Use attach() function to init motors.
 */
DDCMotorWheels::DDCMotorWheels()
{
    MotorSX=nullptr;
    MotorDX=nullptr;
	Attached=false;
    Engaged=false;
    CurrStepValue=DEFAULT_INCREMENTAL_VALUE;
    CurrSXVel=0;
    CurrDXVel=0;
}

/**
 * @brief Construct a new DDCMotorWheels::DDCMotorWheels object and initialize motors.
 * 
 * @param SXPwmPin      ->  In PHASE_ENABLE mode pin is xENABLE(PWM).
 *                          In IN_IN mode pin is PWM pin.
 * @param SXDirPwmPin   ->  In PHASE_ENABLE mode pin is xPHASE(DIR).
 *                          In IN_IN mode pin is PWM pin.
 * @param DXPwmPin      ->  In PHASE_ENABLE mode pin is xENABLE(PWM).
 *                          In IN_IN mode pin is PWM pin.
 * @param DXDirPwmPin   ->  In PHASE_ENABLE mode pin is xPHASE(DIR).
 *                          In IN_IN mode pin is PWM pin.
 * @param Mode          ->  The IN_IN mode all 4 pins are used in pwm mode.
 *                          The PHASE_ENABLE mode only 2 pin are used pwm mode (use it with board like "Pololu DRV8825 Dual Motor Driver Kit")
 */
DDCMotorWheels::DDCMotorWheels(unsigned short int SXPwmPin, unsigned short int SXDirPwmPin, unsigned short int DXPwmPin, unsigned short int DXDirPwmPin, DDCMotor::DControlMode Mode)
{
    MotorSX=nullptr;
    MotorDX=nullptr;
	Attached=false;
    Engaged=false;
    CurrStepValue=DEFAULT_INCREMENTAL_VALUE;
    CurrSXVel=0;
    CurrDXVel=0;
    attach(SXPwmPin,SXDirPwmPin,DXPwmPin,DXDirPwmPin,Mode);
}

/**
 * @brief Construct a new DDCMotorWheels::DDCMotorWheels object using previously created DDCMotor objects.
 * WARNING: both DDCMotor objects will be deleted when this class will be destroyed or when detach() function is called.
 * 
 * @param SXMotor   ->  Pointer to DDCMotor object for sx wheels.
 * @param DXMotor   ->  Pointer to DDCMotor object for dx wheels.
 */
DDCMotorWheels::DDCMotorWheels(DDCMotor *SXMotor, DDCMotor *DXMotor)
{
    MotorSX=SXMotor;
    MotorDX=DXMotor;
    Attached=MotorSX->attached() & MotorDX->attached();
    Engaged=false;
    CurrStepValue=DEFAULT_INCREMENTAL_VALUE;
    MotorSX->SetIncValue(CurrStepValue);
    MotorDX->SetIncValue(CurrStepValue);
}

/**
 * @brief Destroy the DDCMotorWheels::DDCMotorWheels object
 * 
 */
DDCMotorWheels::~DDCMotorWheels()
{
    if (Attached) {
        if (MotorSX) delete MotorSX;
        if (MotorDX) delete MotorDX;
    }
}

/**
 * @brief Initialize hardware pins for motores.
 * 
 * @param SXPwmPin      ->  In PHASE_ENABLE mode pin is xENABLE(PWM).
 *                          In IN_IN mode pin is PWM pin.
 * @param SXDirPwmPin   ->  In PHASE_ENABLE mode pin is xPHASE(DIR).
 *                          In IN_IN mode pin is PWM pin.
 * @param DXPwmPin      ->  In PHASE_ENABLE mode pin is xENABLE(PWM).
 *                          In IN_IN mode pin is PWM pin.
 * @param DXDirPwmPin   ->  In PHASE_ENABLE mode pin is xPHASE(DIR).
 *                          In IN_IN mode pin is PWM pin.
 * @param Mode          ->  The IN_IN mode all 4 pins are used in pwm mode.
 *                          The PHASE_ENABLE mode only 2 pin are used pwm mode (use it with board like "Pololu DRV8825 Dual Motor Driver Kit")
 */
void DDCMotorWheels::attach(unsigned short int SXPwmPin, unsigned short int SXDirPwmPin, unsigned short int DXPwmPin, unsigned short int DXDirPwmPin, DDCMotor::DControlMode Mode)
{

    detach();
    MotorSX=new DDCMotor(SXPwmPin,SXDirPwmPin,Mode);
    MotorDX=new DDCMotor(DXPwmPin,DXDirPwmPin,Mode);
    Attached=MotorSX->attached() & MotorDX->attached();
    CurrSXVel=0;
    CurrDXVel=0;
}

/**
 * @brief Free hardware resources
 * 
 */
void DDCMotorWheels::detach(void)
{
    if (MotorSX) delete MotorSX;
    if (MotorDX) delete MotorDX;
    Attached=false;
    Engaged=false;
}

// ************************* Direct motor output functions *************************

/**
 * @brief Set wheels balancing value for forward direction.
 * A negative value means that sx wheel runs faster than dx, so, MotorDX::PwmLimitFw will decrease (but not under DMotor::MIN_CAL_PWM).
 * A positive value means that dx wheel runs faster than sx, so, MotorSX::PwmLimitFw will decrease (but not under DMotor::MIN_CAL_PWM).
 * A zero value means that sx and dx wheel runs without compensation.
 * 
 * @param Value ->  from -[DDCMotor::MAX_PWM-DDCMotor::MIN_CAL_PWM] to [DDCMotor::MAX_PWM-DDCMotor::MIN_CAL_PWM]. Default: -155 to 155
 */
void DDCMotorWheels::SetBalancingFw(short int Value)
{
    // Limit for calibration value
    unsigned short Limit=DDCMotor::MAX_PWM-DDCMotor::MIN_CAL_PWM;
    if (Value > Limit) Value=Limit;
    else if (Value < -Limit) Value=-Limit;

    // Balancing=CalLimitSX-CalLimitDX
    // so...
    // CalLimitDX=CalLimitSX-Balancing
    // CalLimitSX=CalLimitDX+Balancing
    if (Value > 0) {
        // To DX:
        // SX no limit
        // DX decrease limit
        MotorSX->ResetCalLimitFw();
        MotorDX->SetCalLimitFw(MotorSX->GetCalLimitFw()-Value);
    }
    else if (Value < 0) {
        // To SX:
        // SX decrese balancing
        // DX no limit
        MotorSX->SetCalLimitFw(MotorDX->GetCalLimitFw()+Value);
        MotorDX->ResetCalLimitFw();
    }
    else {
        MotorSX->ResetCalLimitFw();
        MotorDX->ResetCalLimitFw();
    }
}

/**
 * @brief Move balancing FW to SX by Value
 * 
 * @param Value ->  from 1 to 255 (default 1)
 */
void DDCMotorWheels::MoveBalancingFwSX(unsigned short int Value)
{
    SetBalancingFw(GetBalancingFw()-1);
}

/**
 * @brief Move balancing FW to DX by Value
 * 
 * @param Value ->  from 1 to 255 (default 1)
 */
void DDCMotorWheels::MoveBalancingFwDX(unsigned short int Value)
{
    SetBalancingFw(GetBalancingFw()+1);
}

/**
 * @brief Move balancing REV to SX by Value
 * 
 * @param Value ->  from 1 to 255 (default 1)
 */
void DDCMotorWheels::MoveBalancingRevSX(unsigned short int Value)
{
    SetBalancingRev(GetBalancingRev()-1);
}

/**
 * @brief Move balancing REV to DX by Value
 * 
 * @param Value ->  from 1 to 255 (default 1)
 */
void DDCMotorWheels::MoveBalancingRevDX(unsigned short int Value)
{
    SetBalancingRev(GetBalancingRev()+1);
}

/**
 * @brief Set wheels balancing value for backward direction.
 * A negative value means that sx wheel runs faster than dx.
 * A positive value means that dx wheel runs faster than sx.
 * A zero value means that sx and dx wheel runs without compensation.
 * 
 * @param Value ->  from -[DDCMotor::MAX_PWM-DDCMotor::MIN_CAL_PWM] to [DDCMotor::MAX_PWM-DDCMotor::MIN_CAL_PWM]. Default: -155 to 155
 */
void DDCMotorWheels::SetBalancingRev(short int Value)
{
    // Limits for calibration value
    unsigned short Limit=DDCMotor::MAX_PWM-DDCMotor::MIN_CAL_PWM;
    if (Value > Limit) Value=Limit;
	if (Value < -Limit) Value=-Limit;


    // Balancing=CalLimitSX-CalLimitDX
    // so...
    // CalLimitDX=CalLimitSX-Balancing
    // CalLimitSX=CalLimitDX+Balancing
    if (Value > 0) {
        // To DX:
        // SX no limit
        // DX decrease limit
        MotorSX->ResetCalLimitRev();
        MotorDX->SetCalLimitRev(MotorSX->GetCalLimitRev()-Value);
    }
    else if (Value < 0) {
        // To SX:
        // SX decrease limit
        // DX no limit
        MotorSX->SetCalLimitRev(MotorDX->GetCalLimitRev()+Value);
        MotorDX->ResetCalLimitRev();
    }
    else {
        MotorSX->ResetCalLimitRev();
        MotorDX->ResetCalLimitRev();
    }
}

/**
 * @brief Reset all balancing values to 0.
 * 
 */
void DDCMotorWheels::ResetBalancing(void)
{
    MotorSX->ResetCalLimit();
    MotorDX->ResetCalLimit();
}

/**
 * @brief Reset balancing value for forward direction to 0.
 * 
 */
void DDCMotorWheels::ResetBalancingFw(void)
{
    MotorSX->ResetCalLimitFw();
    MotorDX->ResetCalLimitFw();
}

/**
 * @brief Reset balancing value for backward direction to 0.
 * 
 */
void DDCMotorWheels::ResetBalancingRev(void)
{
    MotorSX->ResetCalLimitRev();
    MotorDX->ResetCalLimitRev();
}

/**
 * @brief Set step value used by functions for direct increment - decrecrement speed.
 * 
 * @param Value ->  Valid step values: from 1 to MAX_VEL (a zero value is set to 1)
 */
void DDCMotorWheels::SetStepValue(unsigned short int Value)
{
    MotorSX->SetIncValue(Value);
    MotorDX->SetIncValue(Value);
}

/**
 * @brief Set velocity value for sx motor.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrSXVel is set.
 * 
 * @param Vel    ->  Absolute velocity value from -100 to 100. A value of 0 stops the motor.
 */
void DDCMotorWheels::SXSetVel(short int Vel)
{
    if (Vel > DDCMotor::MAX_VEL) {
        CurrSXVel=DDCMotor::MAX_VEL;
    }
    else {
        CurrSXVel=Vel;
    }

    if (Engaged) {
        MotorSX->Set(CurrSXVel);
    }
}

/**
 * @brief Set velocity value for dx motor.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrSXVel is set.
 * 
 * @param Vel    ->  Absolute velocity value from -100 to 100. A value of 0 stops the motor.
 */
void DDCMotorWheels::DXSetVel(short int Vel)
{
    if (Vel > DDCMotor::MAX_VEL) {
        CurrDXVel=DDCMotor::MAX_VEL;
    }
    else {
        CurrDXVel=Vel;
    }

    if (Engaged) {
        MotorDX->Set(CurrDXVel);
    }
}

/**
 * @brief Stop sx motor (set vel to 0)
 */
void DDCMotorWheels::SXStop(void)
{
	SXSetVel(0);
}

/**
 * @brief Stop dx motor (set vel to 0)
 */
void DDCMotorWheels::DXStop(void)
{
	DXSetVel(0);
}

/**
 * @brief Brake sx motor (set pins level to brake motor)
 */
void DDCMotorWheels::SXBrake(void)
{
	MotorSX->Brake();
}

/**
 * @brief Brake dx motor (set pins level to brake motor)
 */
void DDCMotorWheels::DXBrake(void)
{
	MotorDX->Brake();
}
// *********************End direct motor output functions *************************

// *********************** Direct motor input functions ***************************

/**
* @return Current PWM value for sx motor.
**/
short int DDCMotorWheels::SXGetPwm(void)
{
	return(MotorSX->GetPwm());
}

/**
* @return Current PWM value for dx motor.
**/
short int DDCMotorWheels::DXGetPwm(void)
{
	return(MotorSX->GetPwm());
}

/**
* @return true if sx motor is running.
**/
bool DDCMotorWheels::SXIsRunning(void)
{
    return(MotorSX->IsRunning());
}

/**
* @return true if dx motor is running.
**/
bool DDCMotorWheels::DXIsRunning(void)
{
    return(MotorDX->IsRunning());
}

/**
* @return Currently balancing value for forward direction.
**/
short int DDCMotorWheels::GetBalancingFw(void)
{
    return(MotorSX->GetCalLimitFw() - MotorDX->GetCalLimitFw());
}

/**
* @return Currently balancing value for backward direction.
**/
short int DDCMotorWheels::GetBalancingRev(void)
{
    return(MotorSX->GetCalLimitRev() - MotorDX->GetCalLimitRev());
}
// *********************End direct motor input functions **************************

/**
* @return true if hardware pins are correctly set.
**/
bool DDCMotorWheels::attached(void)
{
    return(Attached);
}

/**
 * @brief Engage/disengage motors.
 *
 * @param Engage    ->  If true all changes of CurrSXVel and CurrDXVel are applied to the motors, otherwise only values are set.
 * N.B. If Engaged is false and motors are runnings, they will be stopped.
 */
void DDCMotorWheels::engage(bool Engage)
{
    Engaged=Engage;
    if (!Engaged) Stop();
}

/**
 * @brief Disengage motors.
 * N.B. If motors are runnings, they will be stopped.
 */
void DDCMotorWheels::disengage(void)
{
    engage(false);
}

/**
 * @return true if motors are engaged.
 */
bool DDCMotorWheels::engaged(void)
{
    return(Engaged);
}

/**
* @return Current sx motor vel value (from -100 to 100).
**/
short int DDCMotorWheels::SXGetVel(void)
{
	return(CurrSXVel);
}

/**
* @return Current dx motor vel value (from -100 to 100).
**/
short int DDCMotorWheels::DXGetVel(void)
{
	return(CurrDXVel);
}

/**
 * @brief Runs sx motor in forward direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrSXVel is set.
 * 
 * @param Vel Speed value from 1 to 100. If Vel is 0 CurrSXVel is used.
 */
void DDCMotorWheels::SXFw(unsigned short int Vel)
{
    if (Vel == 0) {
        SXSetVel(abs(CurrSXVel));
    }
    else {
	    SXSetVel(Vel);
    }
}

/**
 * @brief Runs dx motor in forward direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrDXVel is set.
 * 
 * @param Vel Speed value from 1 to 100. If Vel is 0 CurrDXVel is used.
 */
void DDCMotorWheels::DXFw(unsigned short int Vel)
{
    if (Vel == 0) {
        DXSetVel(abs(CurrDXVel));
    }
    else {
	    DXSetVel(Vel);
    }
}

/**
 * @brief Runs sx motor in backward direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrSXVel is set.
 * 
 * @param Vel Speed value from 1 to 100. If Vel is 0 CurrSXVel is used.
 */
void DDCMotorWheels::SXRev(unsigned short int Vel)
{
    if (Vel == 0) {
        SXSetVel(-abs(CurrSXVel));
    }
    else {
	    SXSetVel(-Vel);
    }
}

/**
 * @brief Runs dx motor in backward direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrDXVel is set.
 * 
 * @param Vel Speed value from 1 to 100. If Vel is 0 CurrDXVel is used.
 */
void DDCMotorWheels::DXRev(unsigned short int Vel)
{
    if (Vel == 0) {
        DXSetVel(-abs(CurrDXVel));
    }
    else {
	    DXSetVel(-Vel);
    }
}

/**
 * @brief Runs both motors in forward direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motors, otherwise only values are set.
 * 
 * @param Vel Speed value from 1 to 100. If Vel is 0 current vel are used.
 */
void DDCMotorWheels::Fw(unsigned short int Vel)
{
    SXFw(Vel);
    DXFw(Vel);
}

/**
 * @brief Runs both motors in backward direction at specific speed value.
 * N.B. If Engaged is true the value is applied to motors, otherwise only values are set.
 * 
 * @param Vel Speed value from 1 to 100. If Vel is 0 current vel are used.
 */
void DDCMotorWheels::Rev(unsigned short int Vel)
{
    SXRev(Vel);
    DXRev(Vel);
}

/**
 * @brief Stop both motor (set vel to 0)
 */
void DDCMotorWheels::Stop(void)
{
	SXStop();
    DXStop();
}

/**
 * @brief Brake both motor (set pins level to brake motors)
 */
void DDCMotorWheels::Brake(void)
{
	SXBrake();
    DXBrake();
}

/**
 * @brief Increment both motors speed by specific value.
 * N.B. If Engaged is true the value is applied to motors, otherwise only values are set.
 * 
 * @param Value ->  Step value, if Value is 0 CurrStepValue is used.
 */
void DDCMotorWheels::IncVel(unsigned short Value)
{
    SXIncVel(Value);
    DXIncVel(Value);
}

/**
 * @brief Decrement both motors speed by specific value.
 * N.B. If Engaged is true the value is applied to motors, otherwise only values are set.
 * 
 * @param Value ->  Step value, if Value is 0 CurrStepValue is used.
 */
void DDCMotorWheels::DecVel(unsigned short Value)
{
    SXDecVel(Value);
    DXDecVel(Value);
}

/**
 * @brief Increment sx motor speed by specific value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrSXVel is set.
 * 
 * @param Value ->  Step value, if Value is 0 CurrStepValue is used.
 */
void DDCMotorWheels::SXIncVel(unsigned short int Value)
{
    if (Value == 0) Value=CurrStepValue;

    if (CurrSXVel < 0) {
        SXSetVel(CurrSXVel-Value);
    }
    else {
        // CurrSXVel >= 0
        SXSetVel(CurrSXVel+Value);
    }
}

/**
 * @brief Decrement sx motor speed by specific value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrSXVel is set.
 * 
 * @param Value ->  step value, if Value is 0 CurrStepValue is used.
 */
void DDCMotorWheels::SXDecVel(unsigned short int Value)
{
    if (Value == 0) Value=CurrStepValue;

    if (CurrSXVel > 0) {
        SXSetVel(CurrSXVel-Value);
    }
    else if (CurrSXVel < 0) {
        SXSetVel(CurrSXVel+Value);
    }
}

/**
 * @brief Increment dx motor speed by specific value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrDXVel is set.
 * 
 * @param Value ->  Step value, if Value is 0 CurrStepValue is used.
 */
void DDCMotorWheels::DXIncVel(unsigned short int Value)
{
    if (Value == 0) Value=CurrStepValue;

    if (CurrDXVel < 0) {
        DXSetVel(CurrDXVel-Value);
    }
    else {
        // CurrDXVel >= 0
        DXSetVel(CurrDXVel+Value);
    }
}

/**
 * @brief Decrement dx motor speed by specific value.
 * N.B. If Engaged is true the value is applied to motor, otherwise only CurrDXVel is set.
 * 
 * @param Value ->  Step value, if Value is 0 CurrStepValue is used.
 */
void DDCMotorWheels::DXDecVel(unsigned short int Value)
{
    if (Value == 0) Value=CurrStepValue;
    
    if (CurrDXVel > 0) {
        DXSetVel(CurrDXVel-Value);
    }
    else if (CurrDXVel < 0) {
        DXSetVel(CurrDXVel+Value);
    }
}

/**
 * @brief Set absolute vel values for both motors.
 * Set -100 for full reverse speed.
 * Set 100 for full forward speed.
 * Set 0 to stop.
 * N.B. If Engaged is true the value is applied to motors, otherwise only values are set.
 * 
 * @param SXValue  ->  Absolute velocity value for sx motor: from -100 to 100, 0 for stop.
 * @param DXValue  ->  Absolute velocity value for dx motor: from -100 to 100, 0 for stop.
 *
 * es:
 * @code
 * Set(50,-100); // SX motor forward at 50% and DX motor reverse at 100%.
 * @endcode
 */
void DDCMotorWheels::SetVel(short int SXVel, short int DXVel)
{
    SXSetVel(SXVel);
    DXSetVel(DXVel);
}