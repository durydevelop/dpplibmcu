/*
 * DigitalOutput.h
 *
 *  Created on: 10/mag/2013
 *      Author: dury
 */

#ifndef DDigitalOutputH
#define DDigitalOutputH

#include <dgpio>

class DDigitalOutput
{
	public:
        #ifndef ARDUINO
            DDigitalOutput(int digitalPin, DGpioHandle gpioHandle = -1);
            ~DDigitalOutput();
        #else
            DDigitalOutput(int digitalPin);
        #endif
        
        bool begin(int initialLevel = LOW);
        int read(void);
		int write(int level);
		void high(void);
		void low(void);
		void toggle(void);
        int getPin(void);
        bool isAttached(void);
        #ifndef ARDUINO
            std::string getLastError(void);
        #endif
        
		operator int();
		DDigitalOutput& operator= (bool level);

	private:
		int pin;
		bool currLevel;
        //bool gpioAttached;

        #ifndef ARDUINO
            DGpioHandle handle;
        #endif
        DResult lastResult;
};


#endif
