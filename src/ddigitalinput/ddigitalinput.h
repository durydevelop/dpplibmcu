#ifndef DDigitalInputH
#define DDigitalInputH

#include <dgpio>

class DDigitalInput {

	public:
        #ifdef ARDUINO
            DDigitalInput(int digitalPin);
        #else
            DDigitalInput(int digitalPin, DGpioHandle gpioHandle = -1);
            ~DDigitalInput();
        #endif

        bool begin(bool pullUp = false, unsigned int msecDebounce = 0);
		bool isChanged(short int *newLevel = nullptr);
		bool isChangedToLow(void);
		bool isChangedToHigh(void);
        bool isAttached(void);
		int read(void);
        int getPin(void);
        #ifndef ARDUINO
            std::string getLastError(void);
        #endif
        
		operator int();

	private:
		int pin;
		int currLevel;
		int prevLevel;
		unsigned long currMsec;
		unsigned long prevMsec;
		unsigned long debounceMsec;
        DResult lastResult;

        #ifndef ARDUINO
            DGpioHandle handle;
        #endif
};
#endif
