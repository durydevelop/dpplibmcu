/**
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

DDigitalButton::DDigitalButton(int digitalPin, DGpioHandle gpioHandle)
{
    pin=digitalPin;
    lastResult=DERR_CLASS_NOT_BEGUN;

    if (gpioHandle < 0) {
        // Try init on first device
        lastResult=initGpio(0,handle);
    }
    else {
        if (isGpioReady(gpioHandle)) {
            handle=gpioHandle;
        }
        else {
            lastResult=DERR_GPIO_NOT_READY;
        }
    }

//	begin();
    if (lastResult == DRES_OK) {
        lastResult=DERR_CLASS_NOT_BEGUN;
    }
}

/**
 * @brief Destroy the DDigitalButton::DDigitalButton object and free pin use.
 */
DDigitalButton::~DDigitalButton()
{
    releasePin(pin,handle);
}

/**
 * @brief Constructor
 * @param digitalPin			->	gpio pin.
 * @param pressedState			->	The level for which the button "is considered pressed", be HIGH or LOW.
 * @param pullUp				->	If true the internal pull-up resistor is connected.
 * @param pressedMillis			->	Time in milliseconds after which "press and release" is triggered as PRESSED.
 * @param longPressedMillis		->	Time in milliseconds after which a "keeping press" is triggered as LONG_PRESSED.
 * @param dblPressSpeedMillis	->	Time in milliseconds before which "press, release, press" is triggered as DBL_PRESSED.
 */
bool DDigitalButton::begin(int pressedState, bool pullUp, unsigned int pressedMillis, unsigned int longPressedMillis, unsigned int dblPressSpeedMillis)
{
    pressedLevel=pressedState;
	pressedDuration=pressedMillis;
	longPressedDuration=longPressedMillis;
	dblPressSpeedDuration=dblPressSpeedMillis;
    
    if (handle >= 0) {
        lastResult=initPin(pin,pullUp ? DPinMode::INPUT_PULLUP : DPinMode::INPUT,DPinFlags::NO_FLAGS,handle);
    }

	// Init timers
	releaseMs=millis();
	pressMs=releaseMs;
	pressedMs=releaseMs;

	// Trigger Callback
	callback=NULL;

    if (lastResult == DRES_OK) {
        // Read current input state
    	read();
    }

    return lastResult == DRES_OK;
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
DDigitalButton::DButtonState DDigitalButton::read(void)
{
	bool trig=false;

	if (prevState == RELEASED) {
		currState=RELEASE;
	}
	if (prevState == PRESSED) {
		currState=PRESS;
	}

    lastResult=readPin(pin,handle);
    if (lastResult < 0) {
        return ERROR_STATE;
    }

	if (lastResult == pressedLevel) {
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
DDigitalButton::operator DDigitalButton::DButtonState()
{
	return read();
}

std::string DDigitalButton::getLastError(void)
{
    return getErrorCode(lastResult);
}
