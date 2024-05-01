#ifndef DDCMotorWheelsH
#define DDCMotorWheelsH

#include "DDCMotor.h"

class DDCMotorWheels {

public:
	//static const unsigned short int MIN_BALANCE_VALUE=DDCMotor::MIN_CAL_PWM;
	//static const unsigned short int MAX_BALANCE_VALUE=DDCMotor::MAX_PWM;
	enum DBalanceValues { MAX_SX=-DDCMotor::MIN_CAL_PWM, MAX_DX=DDCMotor::MAX_PWM };

    DDCMotorWheels();
	DDCMotorWheels(unsigned short int SXPwmPin, unsigned short int SXDirPwmPin, unsigned short int DXPwmPin, unsigned short int DXDirPwmPin, DDCMotor::DControlMode Mode = DDCMotor::DIR_PWM);
	DDCMotorWheels(DDCMotor *SXMotor, DDCMotor *DXMotor);
	~DDCMotorWheels();

	// Attach
	void attach(unsigned short int SXPwmPin, unsigned short int SXDirPin, unsigned short int DXPwmPin, unsigned short int DXDirPwmPin, DDCMotor::DControlMode Mode = DDCMotor::DIR_PWM);
	void detach(void);
	bool attached(void);

	// Engaged
	void engage(bool Engage = true);
	void disengage(void);
	bool engaged(void);

	// Balancing
	void SetBalancingFw(short int Value);
	void SetBalancingRev(short int Value);
	void MoveBalancingFwSX(unsigned short int Value);
	void MoveBalancingFwDX(unsigned short int Value);
	void MoveBalancingRevSX(unsigned short int Value);
	void MoveBalancingRevDX(unsigned short int Value);
	short int GetBalancingFw(void);
	short int GetBalancingRev(void);
	void ResetBalancingFw(void);
	void ResetBalancingRev(void);
	void ResetBalancing(void);

	// Get running info
	short int SXGetVel(void);
	short int SXGetPwm(void);
	short int DXGetVel(void);
	short int DXGetPwm(void);
	bool SXIsRunning(void);
	bool DXIsRunning(void);

	// Go Fw
	void SXFw(unsigned short int Vel = 0);
	void DXFw(unsigned short int Vel = 0);
	void Fw(unsigned short int Vel = 0);

	// Go Rev
	void SXRev(unsigned short int Vel = 0);
	void DXRev(unsigned short int Vel = 0);
	void Rev(unsigned short int Vel = 0);

	// Stop and brake
	void SXStop(void);
	void DXStop(void);
	void Stop(void);
	void SXBrake(void);
	void DXBrake(void);
	void Brake(void);

	// Set vel
	void SXIncVel(unsigned short int Value = 0);
	void SXDecVel(unsigned short int Value = 0);
	void DXIncVel(unsigned short int Value = 0);
	void DXDecVel(unsigned short int Value = 0);
	void IncVel(unsigned short int Value = 0);
	void DecVel(unsigned short int Value = 0);
	void SetStepValue(unsigned short int Value);
	void SXSetVel(short int Vel);
	void DXSetVel(short int Vel);
	void SetVel(short int SXVel, short int DXVel);

	DDCMotor *MotorSX;
	DDCMotor *MotorDX;

	private:
		bool Attached;
		bool Engaged;
		short int CurrSXVel;
		short int CurrDXVel;
		unsigned short int CurrStepValue;
};
#endif
