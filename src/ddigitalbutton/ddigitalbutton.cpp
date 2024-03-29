/*! \mainpage
*	Arduino and Raspberry Pi library for handle button press.
* \section intro_sec Introduction
* This library can be used with Arduino or Raspberry Pi, hardware specific api are performed by pre-processor defines.
*
* Futures:
* -Arduino compliant.
* -Raspberry Pi compliant. Both WiringPi and Pigpio library are supported (see includes comments in DDigitalButton.h)
* -In polling mode, all 8 states and triggers can be read and handled:
* -In event mode, 4 triggers fires callback.
*
* State and triggers:
	RELEASE			STATE:		button is not in StatePressed state.
	PRESS			STATE:		button is in StatePressed state.
	PRESSED			TRIGGER:	press-release after PressedMillis time.
	LONG_PRESSED	TRIGGER:	button is kept pressed for longer than LongPressedMillis.
	LONG_PRESSING	STATE:		after LONG_PRESSED, if button still pressed, the state change in LONG_PRESSING until button is released.
	DBL_PRESSED		TRIGGER:	press-release-press within DblPressSpeedMillis time.
	DBL_PRESSING	STATE:		after DBL_PRESSED, if button still pressed, the state change in DBL_PRESSING until button is released.
	RELEASED		TRIGGER:	release after PRESSED, LONG_PRESSED, DBL_PRESSED.
*
* \section usage_sec Usage
*
* @code
*
* @endcode
*/

#include "ddigitalbutton.h"

/**
 * @brief Constructor
 * @param DigitalPin			->	Pin number where button is connected.
 * @param PressedState			->	Can be HIGH or LOW and it is the pin level for which the button is considered to be pressed.
 * @param PullUp				->	If true the internal pull-up resistor is connected.
 * @param PressedMillis			->	Time in milliseconds after which "press and release" is triggered as PRESSED.
 * @param LongPressedMillis		->	Time in milliseconds after which a "keeping press" is triggered as LONG_PRESSED.
 * @param DblPressSpeedMillis	->	Time in milliseconds before which "press, release, press" is triggered as DBL_PRESSED.
 */
DDigitalButton::DDigitalButton(int digitalPin, uint8_t pressedState, bool pullUp, unsigned int pressedMillis, unsigned int longPressedMillis, unsigned int dblPressSpeedMillis)
{
	// User settings
	pin=digitalPin;
	statePressed=pressedState;
	pressedDuration=pressedMillis;
	longPressedDuration=longPressedMillis;
	dblPressSpeedDuration=dblPressSpeedMillis;

	// Init timers
	releaseMs=millis();
	pressMs=releaseMs;
	pressedMs=releaseMs;

	// Trigger Callback
	callback=NULL;

	// Pull-up
	if (pullUp)	{
		initPin(pin,DPinMode::INPUT_PULLUP);
	}
	else {
		initPin(pin,DPinMode::INPUT);
	}

	// Read current input state
	read();
}

/**
 * Enable event mode: set a callback that is called on each TRIGGER state.
 * @param EventCallback	->	Function to call.
 * Callback function must be used in this way:
 * @code
 * void EventCallback(DDigitalButton::State trigger) {
 * 		if (trigger == DDigitalButton::PRESSED) {
 * 			// Do something
 * 		}
 * }
 */
void DDigitalButton::setEventCallback(DCallback eventCallback)
{
	callback=eventCallback;
}

//! @return the current button State.
DDigitalButton::State DDigitalButton::read(void)
{
	bool trig=false;

	if (prevState == RELEASED) {
		currState=RELEASE;
	}
	if (prevState == PRESSED) {
		currState=PRESS;
	}

	if (readPin(pin) == statePressed) {
		pressMs=millis(); // Press time
		if (prevState == RELEASE) {
			if ((pressMs-releaseMs) > 50) {
				currState=PRESS;
			}
		}
		else if (prevState == PRESS) {
			if ((pressMs-pressedMs) < dblPressSpeedDuration) {
				currState=DBL_PRESSED;
				trig=true;
			}
			if ((pressMs-releaseMs) > longPressedDuration) {
				currState=LONG_PRESSED;
				trig=true;
			}
		}
		else if (prevState == LONG_PRESSED) {
			currState=LONG_PRESSING;
		}
		else if (prevState == DBL_PRESSED) {
			currState=DBL_PRESSING;
		}
	}
	else {
		releaseMs=millis(); // Release time
		if (prevState == PRESS) {
			if ((releaseMs-pressMs) > pressedDuration) {
				currState=PRESSED;
				trig=true;
				pressMs=releaseMs;
			}
			else if ((releaseMs-pressMs) > 50) {
				pressedMs=millis();
			}
		}
		else if (prevState == PRESSED || prevState == LONG_PRESSING || prevState == LONG_PRESSED) {
			currState=RELEASED;
			trig=true;
		}
		else {
			currState=RELEASE;
		}
	}

	prevState=currState;
	if (trig) {
		if (callback != NULL) {
			callback(currState);
		}
	}

	return (currState);
}

//! Overload operator ==
DDigitalButton::operator DDigitalButton::State()
{
	return read();
}
