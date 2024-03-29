#ifndef DDigitalInputH
#define DDigitalInputH

class DDigitalInput{

	public:
		DDigitalInput(int digitalPin, bool pullUp = false, unsigned int msecDebounce = 0);
		bool isChanged(short int *newLevel = nullptr);
		bool isChangedToLow(void);
		bool isChangedToHigh(void);
        bool isAttached(void);
		short int read(void);
        short int getPin(void);
        
		operator int();

	private:
		short int pin;
		int currLevel;
		int prevLevel;
		unsigned long currMsec;
		unsigned long prevMsec;
		unsigned long debounceMsec;
        bool gpioAttached;
};
#endif
