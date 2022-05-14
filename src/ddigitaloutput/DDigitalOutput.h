/*
 * DigitalOutput.h
 *
 *  Created on: 10/mag/2013
 *      Author: dury
 */

#ifndef DDigitalOutputH
#define DDigitalOutputH


#ifdef ARDUINO
    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "wiring.h"
        #include "WProgram.h"
    #endif
#else
    #include <stdint.h>
    #include<pigpio.h>
    //#include <wiringPi.h>

    #ifndef __WIRING_PI_H__
        // for NowMillis()
        #include <chrono>
    #endif
#endif

#ifndef HIGH
    #define HIGH 1
#endif // HIGH

#ifndef LOW
    #define LOW 0
#endif // LOW

#ifndef INPUT
    #define INPUT 0
#endif

#ifndef OUTPUT
    #define OUTPUT 1
#endif

#ifndef INPUT_PULLUP
    #define INPUT_PULLUP 2
#endif

class DDigitalOutput
{
	public:
		DDigitalOutput(uint8_t DigitalPin);
		DDigitalOutput(uint8_t DigitalPin, uint8_t State);
        short int Read(void);
		short int Write(uint8_t State);
		void High(void);
		void Low(void);
		void Toggle(void);
        uint8_t GetPin(void);
        bool attached(void);
		operator bool();
		DDigitalOutput& operator= (bool State);

	private:
        bool InitPin(void);
		uint8_t Pin;
		bool CurrState;
        bool Attached;
};


#endif /* DigitalOutput_H_ */
