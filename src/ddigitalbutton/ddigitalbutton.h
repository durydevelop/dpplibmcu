#ifndef DDigitalButton_H
#define DDigitalButton_H

#include <dgpio>

class DDigitalButton{

	public:
		enum DButtonState {
            ERROR_STATE = -1,
            RELEASE,		//! Release			  STATE:	button is not in StatePressed state.
            PRESS,			//! Press			  STATE:	button is in StatePressed state.
            PRESSED,		//! Pressed			TRIGGER:	press-release after PressedMillis time.
            LONG_PRESSED,	//! Long pressed	TRIGGER:	button is kept pressed for longer than LongPressedMillis.
            LONG_PRESSING,	//! Long Pressing	  STATE:	after LONG_PRESSED, if button still pressed, the state change in LONG_PRESSING until button is released.
            DBL_PRESSED,	//! Double pressed	TRIGGER:	press-release-press within DblPressSpeedMillis time.
            DBL_PRESSING,	//! Double pressing	  STATE:	after DBL_PRESSED, if button still pressed, the state change in DBL_PRESSING until button is released.
            RELEASED		//! Released		TRIGGER:	release after PRESSED, LONG_PRESSED, DBL_PRESSED.
		};

		typedef void (*DCallback)(DButtonState);

		DDigitalButton(int digitalPin, DGpioHandle gpioHandle = -1);
        ~DDigitalButton();

        bool begin(int pressedState = LOW, bool pullUp = true, unsigned int pressedMillis = 100, unsigned int longPressedMillis = 1000, unsigned int dblPressSpeedMillis=100);
		void setEventCallback(DCallback eventCallback);
		DDigitalButton::DButtonState read(void);
        std::string getLastError(void);
        
		operator DDigitalButton::DButtonState();

	private:
		uint8_t pin;
		DButtonState currState;
		DButtonState prevState;
		int pressedLevel;

		unsigned long releaseMs;
		unsigned long pressMs;
		unsigned long pressedMs;

		unsigned long pressedDuration;
		unsigned long dblPressSpeedDuration;
		unsigned long longPressedDuration;
		DCallback callback;

        DGpioHandle handle;
        DResult lastResult;
};
#endif
