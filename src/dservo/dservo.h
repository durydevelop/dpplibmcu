#ifndef DServo_H
#define DServo_H

//#include <dgpio>
#include <dpwm>

class DServo {
    public:
        //! Constants
        static const unsigned int DEFAULT_MIN_PULSE_US=1000;
        static const unsigned int DEFAULT_MAX_PULSE_US=2000;

        // ******** Arduino api compatibility methods ********

        //! Unset servo pin as pwm (Arduino api compatibility)
        void detach(void);

        //! Check if servo pin is configurated as pwm (Arduino api compatibility)
        bool attached(void);

        //! Set servo position in Degrees from 0 to 180 where 90 is the centre position (Arduino api compatibility)
        void write(short int degrees);

        //! Set servo position in micro seconds from 1000 to 2000 where 1500 is centre position (Arduino api compatibility)
        void writeMicroseconds(int us);

        //!Read the current angle of the servo, that is the value passed to the last call to write() (Arduino api compatibility)
        short int read(void);

        // ****** End arduino api compatibility methods ******

        //! Constructor 2
        DServo(int pin, DGpioHandle gpioHandle = -1);

        //! Destructor
        ~DServo();

        /// Init servo pina as pwm.
        void begin(void);

        //! Set servo position in range from 0.0 to 1.0 where 0.05 is centre position.
        void writeRange(float pos);

        //! Wrapper for write()
        void writeDegrees(short int degrees);

        //! Move servo fully counterclockwise position MaxPulseUs (180 degrees).
        void fullCCW(void);
        //! Move servo fully clockwise position MinPulseUs (0 degrees).
        void fullCW(void);
        //! Move servo to middle position between MaxPulseUs and MinPulseUs (90 degrees).
        void center(void);
        //! Move servo to home position CurrHomeUs.
        void home(void);

        //! Move servo counterclockwise by custom degrees
        void stepCCWDegrees(short int degreeStep);
        //! Move servo clockwise by custom degrees
        void stepCWDegrees(short int degreeStep);
        //! Move servo counterclockwise by custom pulse-width
        void stepCCWUs(int us);
        //! Move servo clockwise by custom pulse-width
        void stepCWUs(int us);
        //! Move servo counterclockwise by CurrStepUs pulse-width
        void stepCCW(void);
        //! Move servo clockwise by CurrStepUs pulse-width
        void stepCW(void);

        //! Set minimum pulse-witdh that can be set
        void setMinPulseUs(int us);
        //! Set maximum pulse-width that can be set
        void setMaxPulseUs(int us);
        //! Get current maximum pulse-width that can be set
        int getMaxPulseUs(void);
        //! Get current minimum pulse-width that can be set
        int getMinPulseUs(void);

        //! Get current Degrees set
        short int getCurrDegrees(void);
        //! Get current pulse-width set
        int getCurrPulseUs(void);

        //! Set degrees value to use in step moving functions
        void setStepDegrees(short int degrees);
        //! Get current degrees value used in step moving functions
        short int getStepDegrees(void);
        //! Set pulse-width value to use in step moving functions
        void setStepUs(int Us);
        //! Get current pulse-width value used in step moving functions
        int getStepUs(void);

        //! Set degrees value used for home position
        void setHomeDegrees(short int degrees);
        //! Get current pulse-width used for home position.
        short int getHomeDegrees(void);
        //! Set pulse-width used for home position.
        void setHomeUs(int Us);
        //! Get current pulse-width used for home position.
        int getHomeUs(void);

        //! Reset minimum pulse-width to default DEFAULT_MIN_PULSE_US
        int resetMinPulseValue(void);
        //! Reset maxinum pulse-width to default DEFAULT_MAX_PULSE_US
        int resetMaxPulseValue(void);
        //! Reset step pulse-width to default DEFAULT_STEP_VALUE
        int resetStepValue(void);
        //! Reset pulse-width to default: (DEFAULT_MAX_PULSE_US-DEFAULT_MIN_PULSE_US)/2
        int resetHomeValue(void);
        //! Set all params to default values.
        void setDefaultValues(bool outServo);

        //! Convert degrees value to pulse-width Us for this servo
        int degreesToUs(short int degrees);
        //! Convert Us pulse-width value degrees for this servo
        short int usToDegrees(int us);
        //! Get attached pin number
        short int getPin(void);

        std::string getLastError(void);

        /**  TODO: Shorthand for the write and read functions */
        /*
        DServo& operator= (float range);
        DServo& operator= (DServo& rhs);
        operator float();
        */

    private:
        void servoOut();            //! Output pwm signal to servo pin.

        DPwmOut *dPwmOut;
        int pin;

        int currPulseUs;            //! Current pulse-width in us.
        short int currDegrees;      //! Current servo position in degrees from 0 to 180 where 90 is centre position.
        int minPulseUs;             //! Pulse-width that sets servo to fully counter-clockwise position.
        int maxPulseUs;             //! Pulse-width that sets servo to fully clockwise position.
        short int currStepDegrees;  //! Current degrees value used for step.
        int currStepUs;             //! Current pulse-width value used for step.
        int currHomeUs;             //! Current pulse-width value used for home position

        DGpioHandle handle;
};
#endif
