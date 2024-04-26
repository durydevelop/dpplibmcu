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

class DMenu {
	public:
        enum DMenuAction { DMENU_ACTION_SELECT, DMENU_ACTION_UP, DMENU_ACTION_DOWN, DMENU_ACTION_BACK };

		typedef void (*DMenuListener) (DMenu *MenuItem, DMenuAction Action);
		explicit DMenu(const char MenuItemName[], int MenuID=-1, DMenu *Parent=NULL, DMenuListener CallbackFunc=NULL);
		~DMenu();
		void Init(const char MenuItemName[], int MenuID, DMenu *ParentItem, DMenuListener CallbackFunc);
		void SetName(const char ItemName[]);
		bool AddItem(const char Name[]);
        bool AddItem(const char Name[], int ItemID);
        bool AddItem(const char ItemName[], bool SetCurrent);
        bool AddItem(const char Name[], int ItemID, bool SetCurrent);
		DMenu* Up(void);
		DMenu* Down(void);
		DMenu* Back(void);
		DMenu* Select(void);

		DMenu* GetItem(short int Index);
		DMenu* GetCurrItem(void);
		DMenu* GetParent(void);
        bool HasParent(void);
		uint8_t GetItemsCount(void);
		const char* GetName(void);
        bool NameEquals(const char MenuName[]);
		uint8_t GetID(void);
		short int GetCurrItemIndex(void);
		bool Loop;

	private:
		DMenuListener Callback;
		char *Name;
		DMenu *Parent;
		short int ItemIndex; //! Index used to handle the current selected Item
		uint8_t ItemsCount;
		int ID;
		uint8_t MaxItemTextLen;
		DMenu** Items;

};

#endif
