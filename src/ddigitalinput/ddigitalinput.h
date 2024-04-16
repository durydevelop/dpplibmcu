#ifndef DDigitalInputH
#define DDigitalInputH

#include <dgpio>

class DDigitalInput {

	public:
        DDigitalInput(int digitalPin, DGpioHandle gpioHandle = -1);
        ~DDigitalInput();

        bool begin(bool pullUp = false, unsigned int msecDebounce = 0);
		bool isChanged(short int *newLevel = nullptr);
		bool isChangedToLow(void);
		bool isChangedToHigh(void);
        bool isAttached(void);
		int read(void);
        int getPin(void);
        std::string getLastError(void);
        
		operator int();

	private:
		int pin;
		int currLevel;
		int prevLevel;
		unsigned long currMsec;
		unsigned long prevMsec;
		unsigned long debounceMsec;

        DGpioHandle handle;
        DResult lastResult;
};
#endif
