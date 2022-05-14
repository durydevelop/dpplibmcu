#ifndef DDCMotorH
#define DDCMotorH

class DDCMotor {
    public:
        // Control modes
        enum DControlMode { PWM_PWM, DIR_PWM };
        // Rotation direction
        enum DRotationDir { ROT_CC, ROT_CW , ROT_NONE };
        // Constants
        static const short          MAX_VEL=100;     //! Max vel value.
        static const unsigned short MAX_PWM=255;     //! Max pwm value.
        static const unsigned short MIN_CAL_PWM=100; //! Limit pwm value for minimum calibration value.
        static const unsigned short VEL_CURRENT=0;   //! Default value for indicating use current vel

        DDCMotor();
        DDCMotor(unsigned short int PinPwm, unsigned short int PinDirPwm, DControlMode Mode = DIR_PWM);
        ~DDCMotor();

        void attach(unsigned short int PinPwm, unsigned short int PinDirPwm, DControlMode Mode = DIR_PWM);
        void detach(void);
        bool attached(void);

        void SetCalLimitFw(unsigned short int MaxPwmFw);
        void SetCalLimitRev(unsigned short int MaxPwmRev);
        void ResetCalLimit(void);
        void ResetCalLimitRev(void);
        void ResetCalLimitFw(void);
        unsigned short int GetCalLimitFw(void);
        unsigned short int GetCalLimitRev(void);

        void SetIncValue(unsigned short int Value);
        void SwappedDirMode(bool Enabled);

        short int GetVel(void);
        unsigned short int GetPwm(void);
        DRotationDir GetDir(void);

        void CW(unsigned short int Vel = VEL_CURRENT);
        void CC(unsigned short int Vel = VEL_CURRENT);
        void Stop(void);
        void Brake(void);
        void Set(short int Speed);

        bool IsRunning(void);
        unsigned short int GetPinPwm(void);
        unsigned short int GetPinDirPwm(void);

    protected:
        unsigned short PwmPin;
        unsigned short DirPwmPin;
        unsigned short SwappedDir;
        bool Attached;
        bool Running;
        short int CurrVel;
        unsigned short int PwmLimitFw;
        unsigned short int PwmLimitRev;
        DControlMode       ControlMode;
        unsigned short int CurrStepValue;
};
#endif
