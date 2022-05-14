#ifndef DDigitalInputH
#define DDigitalInputH

class DDigitalInput{

	public:
		DDigitalInput(int DigitalPin, bool PullUp = false, unsigned int MsecDebounce = 0);
		void SetPullUp(void);
        #ifdef PIGPIO_VERSION
            void SetPullDown(void);
            void SetFloat(void);
        #endif
		bool IsChanged(short int *NewLevel = nullptr);
		bool IsChangedToLow(void);
		bool IsChangedToHigh(void);
		short int Read(void);
        uint8_t GetPin(void);
        bool attached(void);
		operator int();

	private:
        //bool InitPin(bool PullUp);
        //unsigned long NowMillis(void);
		uint8_t Pin;
		int CurrLevel;
		int PrevLevel;
		unsigned long CurrMsec;
		unsigned long PrevMsec;
		unsigned long DebounceMsec;
        bool Attached;
};
#endif
