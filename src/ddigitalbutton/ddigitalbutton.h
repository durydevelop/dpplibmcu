#ifndef DDigitalButton_H
#define DDigitalButton_H

#include <dgpio>

class DDigitalButton{

	public:
		enum State {RELEASE,		//! Release			  STATE:	button is not in StatePressed state.
					PRESS,			//! Press			  STATE:	button is in StatePressed state.
					PRESSED,		//! Pressed			TRIGGER:	press-release after PressedMillis time.
					LONG_PRESSED,	//! Long pressed	TRIGGER:	button is kept pressed for longer than LongPressedMillis.
					LONG_PRESSING,	//! Long Pressing	  STATE:	after LONG_PRESSED, if button still pressed, the state change in LONG_PRESSING until button is released.
					DBL_PRESSED,	//! Double pressed	TRIGGER:	press-release-press within DblPressSpeedMillis time.
					DBL_PRESSING,	//! Double pressing	  STATE:	after DBL_PRESSED, if button still pressed, the state change in DBL_PRESSING until button is released.
					RELEASED		//! Released		TRIGGER:	release after PRESSED, LONG_PRESSED, DBL_PRESSED.
		};

		typedef void (*DCallback)(State);

		DDigitalButton(int digitalPin, uint8_t pressedState = LOW, bool pullUp = true, unsigned int pressedMillis = 100, unsigned int longPressedMillis = 1000, unsigned int dblPressSpeedMillis=100);
		void setEventCallback(DCallback eventCallback);
		DDigitalButton::State read(void);
		operator DDigitalButton::State();

	private:
		uint8_t pin;
		State currState;
		State prevState;
		uint8_t statePressed;

		unsigned long releaseMs;
		unsigned long pressMs;
		unsigned long pressedMs;

		unsigned long pressedDuration;
		unsigned long dblPressSpeedDuration;
		unsigned long longPressedDuration;
		DCallback callback;
};
#endif
