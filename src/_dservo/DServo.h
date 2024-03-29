#ifndef DSERVO_H
#define DSERVO_H

#include <dsoftpwm>

class DServo {
    public:
        //! Constants
        static const unsigned int DEFAULT_MIN_PULSE_US=1000;
        static const unsigned int DEFAULT_MAX_PULSE_US=2000;

        // ******** Arduino api compatibility methods ********

        //! Constructor 1 (Arduino api compatibility)
        DServo();

        //! Set servo pin as pwm (Arduino api compatibility)
        void attach(int Pin);

        //! Unset servo pin as pwm (Arduino api compatibility)
        void detach(void);

        //! Check if servo pin is configurated as pwm (Arduino api compatibility)
        bool attached(void);

        //! Set servo position in Degrees from 0 to 180 where 90 is the centre position (Arduino api compatibility)
        void write(short int Degrees);

        //! Set servo position in micro seconds from 1000 to 2000 where 1500 is centre position (Arduino api compatibility)
        void writeMicroseconds(int Us);

        //!Read the current angle of the servo, that is the value passed to the last call to write() (Arduino api compatibility)
        short int read(void);

        // ****** End arduino api compatibility methods ******

        //! Constructor 2
        DServo(short int Pin);

        //! Destructor
        ~DServo();

        //! Set servo position in range from 0.0 to 1.0 where 0.05 is centre position.
        void WriteRange(float Pos);

        //! Wrapper for write()
        void WriteDegrees(short int Degrees);

        //! Wrapper for writeMicroseconds()
        void WriteMicroseconds(int Us);

        //! Move servo fully counterclockwise position MaxPulseUs (180 degrees).
        void FullCCW(void);
        //! Move servo fully clockwise position MinPulseUs (0 degrees).
        void FullCW(void);
        //! Move servo to middle position between MaxPulseUs and MinPulseUs (90 degrees).
        void Center(void);
        //! Move servo to home position CurrHomeUs.
        void Home(void);

        //! Move servo counterclockwise by custom degrees
        void StepCCWDegrees(short int DegreeStep);
        //! Move servo clockwise by custom degrees
        void StepCWDegrees(short int DegreeStep);
        //! Move servo counterclockwise by custom pulse-width
        void StepCCWUs(int Us);
        //! Move servo clockwise by custom pulse-width
        void StepCWUs(int Us);
        //! Move servo counterclockwise by CurrStepUs pulse-width
        void StepCCW(void);
        //! Move servo clockwise by CurrStepUs pulse-width
        void StepCW(void);

        //! Set minimum pulse-witdh that can be set
        void SetMinPulseUs(int Us);
        //! Set maximum pulse-width that can be set
        void SetMaxPulseUs(int Us);
        //! Get current maximum pulse-width that can be set
        int GetMaxPulseUs(void);
        //! Get current minimum pulse-width that can be set
        int GetMinPulseUs(void);

        //! Get current Degrees set
        short int GetCurrDegrees(void);
        //! Get current pulse-width set
        int GetCurrPulseUs(void);

        //! Set degrees value to use in step moving functions
        void SetStepDegrees(short int Degrees);
        //! Get current degrees value used in step moving functions
        short int GetStepDegrees(void);
        //! Set pulse-width value to use in step moving functions
        void SetStepUs(int Us);
        //! Get current pulse-width value used in step moving functions
        int GetStepUs(void);

        //! Set degrees value used for home position
        void SetHomeDegrees(short int Degrees);
        //! Get current pulse-width used for home position.
        short int GetHomeDegrees(void);
        //! Set pulse-width used for home position.
        void SetHomeUs(int Us);
        //! Get current pulse-width used for home position.
        int GetHomeUs(void);

        //! Reset minimum pulse-width to default DEFAULT_MIN_PULSE_US
        int ResetMinPulseValue(void);
        //! Reset maxinum pulse-width to default DEFAULT_MAX_PULSE_US
        int ResetMaxPulseValue(void);
        //! Reset step pulse-width to default DEFAULT_STEP_VALUE
        int ResetStepValue(void);
        //! Reset pulse-width to default: (DEFAULT_MAX_PULSE_US-DEFAULT_MIN_PULSE_US)/2
        int ResetHomeValue(void);
        //! Set all params to default values.
        void SetDefaultValues(bool OutServo);

        //! Convert degrees value to pulse-width Us for this servo
        int DegreesToUs(short int Degrees);
        //! Convert Us pulse-width value degrees for this servo
        short int UsToDegrees(int Us);
        //! Get attached pin number
        short int GetServoPin(void);

        /**  TODO: Shorthand for the write and read functions */
        /*
        DServo& operator= (float range);
        DServo& operator= (DServo& rhs);
        operator float();
        */

    private:
        void ServoOut();            //! Output pwm signal to servo pin.

        DPwmOut *dPwmOut;

        int CurrPulseUs;            //! Current pulse-width in us.
        short int CurrDegrees;      //! Current servo position in degrees from 0 to 180 where 90 is centre position.
        int MinPulseUs;             //! Pulse-width that sets servo to fully counter-clockwise position.
        int MaxPulseUs;             //! Pulse-width that sets servo to fully clockwise position.
        short int CurrStepDegrees;  //! Current degrees value used for step.
        int CurrStepUs;             //! Current pulse-width value used for step.
        int CurrHomeUs;             //! Current pulse-width value used for home position
};
#endif
