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
        DDigitalOutput(int digitalPin, DGpioHandle gpioHandle = -1);
        ~DDigitalOutput();

        bool begin(int initialLevel = LOW);
        int read(void);
		int write(int level);
		void high(void);
		void low(void);
		void toggle(void);
        int getPin(void);
        bool isAttached(void);
        std::string getLastError(void);
        
		operator int();
		DDigitalOutput& operator= (bool level);

	private:
		int pin;
		bool currLevel;
        //bool gpioAttached;

        DGpioHandle handle;
        DResult lastResult;
};


#endif
