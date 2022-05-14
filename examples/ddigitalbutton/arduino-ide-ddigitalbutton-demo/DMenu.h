/*
 *  Class:      DMenu
 *  Created on: 2018
 *  Author:     dury
 *  Version:    1.0
 */

#ifndef DMENU_H
#define DMENU_H

//#include "mbed.h"

#if defined(ARDUINO)
    #if ARDUINO >= 100
        #include "Arduino.h"
    #else
        #include "wiring.h"
        #include "WProgram.h"
    #endif
#else
	#include <cstring>
	#include <cstdlib>
	using namespace std;
#endif // defined

#ifndef BYTE
	typedef unsigned char BYTE;
#endif

// DMenu defines
#define DMENU_DEFAULT_MAX_ITEM_TEXT_LEN 21
#define DMENU_MAX_ITEMS 255
#define DMENU_ACTION_UP     1
#define DMENU_ACTION_DOWN   2
#define DMENU_ACTION_BACK   3
#define DMENU_ACTION_SELECT 4

class DMenu {
	public:
		typedef void (*DMenuListener) (DMenu *MenuItem, BYTE Action);
		DMenu(const char MenuItemName[], DMenuListener CallbackFunc=NULL, DMenu *Parent=NULL);
		~DMenu();
		void Init(const char MenuItemName[], DMenuListener CallbackFunc, DMenu *ParentItem);
		void SetName(const char ItemName[]);
		short int AddItem(const char Name[], bool SetCurrent=false);
		DMenu* Up(void);
		DMenu* Down(void);
		DMenu* Back(void);
		DMenu* Select(void);

		DMenu* GetItem(short int Index);
		DMenu* GetCurrItem(void);
		DMenu *GetParent(void);
		short int GetItemsCount(void);
		const char* GetName(void);
		short int GetID(void);
		short int GetCurrItemIndex(void);
		bool Loop;

	private:
		DMenuListener Callback;
		char *Name;
		DMenu *Parent;
		short int ItemIndex; //! Index used to handle the current selected Item
		short int ItemsCount;
		short int ID;
		BYTE MaxItemTextLen;
		DMenu** Items;

};

#endif
