/*
 * DigitalOutput.h
 *
 *  Created on: 10/mag/2013
 *      Author: dury
 */

#ifndef DDigitalOutputH
#define DDigitalOutputH

#include<dgpio>

class DDigitalOutput
{
	public:
		DDigitalOutput(uint8_t digitalPin);
		DDigitalOutput(uint8_t digitalPin, uint8_t initialLevel);
        short int read(void);
		short int write(uint8_t level);
		void high(void);
		void low(void);
		void toggle(void);
        uint8_t getPin(void);
        bool isAttached(void);
		operator bool();
		DDigitalOutput& operator= (bool level);

	private:
		uint8_t pin;
		bool currLevel;
        bool gpioAttached;
};


#endif
