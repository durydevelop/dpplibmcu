#include "dpplib-gpio.h"

// TODO: USE_DSOFT_PWM

bool InitPin(uint8_t Pin, uint8_t Mode)
{
    #ifdef ARDUINO
        if (Mode == OUTPUT_PWM_MODE) {
            #ifdef USE_DSOFT_PWM
                // TODO: dPwmOut=new DPwmOut(Pin);
            #else
                pinMode(Pin,OUTPUT);
            #endif
        }
        else {
            pinMode(Pin,Mode);
        }
    #else
        // Rpi
        #ifdef PIGPIO_VERSION
            // Init lib
            if (gpioInitialise() == PI_INIT_FAILED) {
                return false;
            }

            // Set mode
            if (Mode == OUTPUT_PWM) {
                gpioSetMode(Pin,PI_OUTPUT);
            }
            else {
                gpioSetMode(Pin,Mode);
            }
        #else
            // WiringPi
            pinMode(Pin,Mode);
        #endif
    #endif
    
    return true;
}

/**
 * @brief Read pin level.
 * 
 * @param Pin 
 * @return pin level (HIGH or LOW)
 */
uint8_t ReadPin(uint8_t Pin) {
    #ifdef ARDUINO
		return(digitalRead(Pin));
    #else
        // Rpi
        #ifdef PIGPIO_VERSION
			return(gpioRead(Pin));
        #else
			return(digitalRead(Pin));
        #endif
    #endif
}

void WritePin(uint8_t Pin, uint8_t Level)
{
    #ifdef ARDUINO
		digitalWrite(Pin,Level);
    #else
        // Rpi
        #ifdef PIGPIO_VERSION
			gpioWrite(Pin,Level);
        #else
			digitalWrite(Pin,Level);
        #endif
    #endif
}

/**
 * @brief Output a pwm signal to pin.
 * 
 * PWM output only works on the pins with
 * hardware support.  These are defined in the appropriate
 * pins_*.c file.  For the rest of the pins, we default
 * to digital output.
 * 
 * @param Pin    ->  pwm pin
 * @param Value  ->  pwm value from 0 to MAX_PWM
 */
void WriteAnalog(unsigned short int Pin, unsigned short int Value)
{
    #ifdef ARDUINO
        #ifdef USE_DSOFT_PWM
            // TODO
        #else
            analogWrite(Pin,Value);
        #endif
    #else
        #ifdef PIGPIO_VERSION
            gpioPWM(Pin,Value);
        #else
            #ifdef USE_DSOFT_PWM
                // TODO
            #else
                analogWrite(Pin,Value);
            #endif
        #endif
    #endif
}

void SetPullUp(short Pin) {
    #ifdef ARDUINO
        digitalWrite(Pin,HIGH);
    #else
        #ifdef PIGPIO_VERSION
            gpioSetPullUpDown(Pin,PI_PUD_UP);
        #else
            digitalWrite(Pin,HIGH);
        #endif
    #endif
}

#ifdef PIGPIO_VERSION
	void DDigitalInput::SetPullDown(short Pin) {
		if (!Attached) return;
		gpioSetPullUpDown(Pin,PI_PUD_DOWN);
	}

	void DDigitalInput::SetFloating(short Pin) {
		if (!Attached) return;
		gpioSetPullUpDown(Pin,PI_PUD_OFF);
	}
#endif

unsigned long Now(void)
{
    #ifdef ARDUINO
        return(millis());
    #else
        #ifdef PIGPIO_VERSION
            // c++ equivalent of arduino millis()
            return(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        #else
            return(millis());
        #endif
    #endif
}

/*
// Posix version
long millis(){
    struct timespec _t;
    clock_gettime(CLOCK_REALTIME, &_t);
    return _t.tv_sec*1000 + lround(_t.tv_nsec/1.0e6);
}
*/