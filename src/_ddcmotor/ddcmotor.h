#ifndef DDCMotorH
#define DDCMotorH

#ifdef ARDUINO
    #ifdef PLATFORMIO
        #include "../dpwm/dpwm"
    #endif
#else
    #include <dsoftpwm>
#endif

class DDCMotor {
    public:
        // Control modes
        enum DControlMode { PWM_PWM, DIR_PWM };
        // Rotation direction
        enum DRotationDir { ROT_CC, ROT_CW , ROT_NONE };
        // Constants
        static const uint8_t          MAX_VEL=100;     //! Max vel value.
        //static const uint16_t MAX_PWM=255;     //! Max pwm value.
        //static const uint16_t MIN_CAL_PWM=100; //! Limit pwm value for minimum calibration value.
        //static const unsigned short VEL_CURRENT=0;   //! Default value for indicating use current vel

        uint16_t MAX_PWM_VALUE=255;     //! Max pwm value (255 is arduino max value for analogWrite()).
        uint16_t MIN_CAL_PWM_VALUE=100; //! Limit pwm value for minimum calibration value.

        DDCMotor();
        DDCMotor(unsigned short int PinDirPwm, unsigned short int PinPwm, DControlMode Mode = DIR_PWM);
        ~DDCMotor();

        void attach(void);
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

        void CW(unsigned short int Vel);
        void CC(unsigned short int Vel);
        void Stop(void);
        void Brake(void);
        void SetVel(int Vel);

        bool IsRunning(void);
        unsigned short int GetPinPwm(void);
        unsigned short int GetPinDirPwm(void);

    protected:
        int CurrVel;
        //unsigned short PwmPin;
        DPwmOut *PwmOut1;
        DPwmOut *PwmOut2;
        //unsigned short DirPwmPin;
        unsigned short SwappedDir;
        //bool Attached;
        bool Running;
        
        unsigned short int PwmLimitFw;
        unsigned short int PwmLimitRev;
        DControlMode       ControlMode;
        unsigned short int CurrStepValue;
};
#endif
