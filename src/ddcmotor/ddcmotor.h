#ifndef DDCMotorH
#define DDCMotorH

#include <dpwm>

class DDCMotor {
    public:
        // Control modes
        enum DControlMode { PWM_PWM, DIR_PWM };
        // Rotation direction
        enum DRotationDir { ROT_CC, ROT_CW , ROT_NONE };
        // Constants
        static const int MAX_VEL=100;     //! Max vel value.
        
        DDCMotor(unsigned short int PinDirPwm, unsigned short int PinPwm, DControlMode Mode = DIR_PWM);
        ~DDCMotor();

        void attach(void);
        void detach(void);
        bool attached(void);

        void SetPwmLimitFw(unsigned short int MaxPwmFw);
        void SetPwmLimitRev(unsigned short int MaxPwmRev);
        void DecPwmLimitFw(void);
        void DecPwmLimitRev(void);
        void IncPwmLimitFw(void);
        void IncPwmLimitRev(void);
        void ResetPwmLimit(void);
        void ResetPwmLimitRev(void);
        void ResetPwmLimitFw(void);
        unsigned short int GetPwmLimitFw(void);
        unsigned short int GetPwmLimitRev(void);
        unsigned short int GetMaxPwmValue(void);

        void SetIncValue(unsigned short int Value);
        void SwappedDirMode(bool Enabled);

        short int GetVel(void);
        unsigned short int GetPwm(void);
        DRotationDir GetDir(void);

        void CW(unsigned short int Vel);
        void CC(unsigned short int Vel);
        void Stop(void);
        void Brake(void);
        void SetVel(int Vel);

        bool IsRunning(void);
        unsigned short int GetPinPwm(void);
        unsigned short int GetPinDirPwm(void);
        unsigned short int VelToPwm(short int Vel);
        //short int PwmToVel(unsigned short int PwmValue);

    protected:
        int CurrVel;
        //unsigned short PwmPin;
        DPwmOut *PwmOut1;
        DPwmOut *PwmOut2;
        //unsigned short DirPwmPin;
        unsigned short SwappedDir;
        //bool Attached;
        bool Running;

        uint16_t maxPwmValue=255;     //! Max pwm value (255 is arduino max value for analogWrite()).
        uint16_t minCalPwmValue=100; //! Limit pwm value for minimum calibration value.
        unsigned short int PwmLimitFw;
        unsigned short int PwmLimitRev;
        DControlMode       ControlMode;
        unsigned short int stepVelValue;
};
#endif
