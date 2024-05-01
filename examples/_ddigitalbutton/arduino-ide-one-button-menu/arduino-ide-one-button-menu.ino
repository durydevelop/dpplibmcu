#include "Arduino.h"
#include "DMenu.h"
#include <U8x8lib.h>
#include "DDigitalButton.h"

#define WIDTH 16
#define HEIGHT 8

#define ID_MAIN_MENU	0
#define ID_SUB_MENU_1	1
#define ID_SUB_MENU_2	2

DMenu* CreateMainMenu(void);
DMenu* CreateSubMenu1(void);
DMenu* CreateSubMenu2(void);
void PrintMenu(DMenu *Menu, uint8_t X = 0, uint8_t Y = 0);
void Debug(const String& msg);

// Display
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
// Button
DDigitalButton *Button=NULL;
// Menu
DMenu *CurrMenu=NULL;

void setup()
{
    // Begin display
	u8x8.begin();
	u8x8.setFont(u8x8_font_chroma48medium8_r);

    // Begin serial debug
	Serial.begin(9600);
	Debug("Start");

    // Create Button
	Button=new DDigitalButton(6);
	Debug("Button on input pin 6");

    // Create MainMenu
	CurrMenu=CreateMainMenu();
    PrintMenu(CurrMenu);
}

// Poll Button and handle read status
void loop()
{
    DMenu *SelItem;

	switch (Button->Read()) {
		case DDigitalButton::PRESSED:
			Debug("<PRESSED>");
			CurrMenu->Down();
            PrintMenu(CurrMenu);
			break;
		case DDigitalButton::LONG_PRESSED: {
			Debug("<LONG_PRESSED>");
			SelItem=CurrMenu->Select();
            String ItemName=SelItem->GetName();
            Debug("Item "+ItemName);
            if (ItemName == "SubMenu 1") {
                CurrMenu=CreateSubMenu1();
                PrintMenu(CurrMenu);
            }
            else if (ItemName == "SubMenu 2") {
                CurrMenu=CreateSubMenu2();
                PrintMenu(CurrMenu);
            }
            else if (ItemName == "Back") {
                if (String(CurrMenu->GetName()) == "SubMenu 1" || String(CurrMenu->GetName()) == "SubMenu 2") {
                    CurrMenu=CreateMainMenu();
                }
                PrintMenu(CurrMenu);
            }
			break;
        }
		case DDigitalButton::DBL_PRESSED:
			//Serial.print("\r\n<DBL_PRESSED>\r\n");
			break;
		case DDigitalButton::RELEASED:
			//Serial.print("\r\n<RELEASED>\r\n");
			break;
		case DDigitalButton::DBL_PRESSING:
			//Serial.print("DBL_PRESSING ");
			break;
		case DDigitalButton::PRESS:
			//Serial.print("PRESS ");
			break;
		case DDigitalButton::LONG_PRESSING:
			//Serial.print("LONG_PRESSING ");
			break;
		case DDigitalButton::RELEASE:
			//Serial.print("RELEASE ");
			break;
		default:
			break;
	}
}

DMenu* CreateMainMenu(void) {
	if (CurrMenu != NULL) {
        // Only one menu at time
		delete CurrMenu;
	}

	CurrMenu=new DMenu("Main Menu");
	CurrMenu->Loop=true;

	CurrMenu->AddItem("SubMenu 1",true);
	CurrMenu->AddItem("SubMenu 2");

    return(CurrMenu);
}

DMenu* CreateSubMenu1(void) {
	if (CurrMenu != NULL) {
        // Only one menu at time
		delete CurrMenu;
	}

	CurrMenu=new DMenu("SubMenu 1");
	CurrMenu->Loop=true;

	CurrMenu->AddItem("Execute",true);
	CurrMenu->AddItem("Back");

    return(CurrMenu);
}

DMenu* CreateSubMenu2(void) {
	if (CurrMenu != NULL) {
        // Only one menu at time
		delete CurrMenu;
	}

	CurrMenu=new DMenu("SubMenu 2");
	CurrMenu->Loop=true;

	CurrMenu->AddItem("Execute",true);
	CurrMenu->AddItem("Back");

    return(CurrMenu);
}

void PrintMenu(DMenu* Menu, uint8_t X, uint8_t Y)
{
    // Line buffer
	char Line[WIDTH+1];
    memset(Line,' ',WIDTH+1);

    // Create centered menu title line
    uint8_t nameLen=strlen(Menu->GetName());
    uint8_t startCol=(WIDTH-nameLen)/2;
	memcpy(&Line[startCol],Menu->GetName(),nameLen);
    Line[WIDTH]='\0';

    // Print menu title
	u8x8.drawString(X,Y,Line);

    // Starts from second line
	for (uint8_t iY=Y; iY<Menu->GetItemsCount();iY++) {
        DMenu* Item=CurrMenu->GetItem(iY);
        // Starts with item name 
        uint8_t itemLen=strlen(Item->GetName());
		memcpy(Line,Item->GetName(),itemLen);
        // Fill with spaces
		memset(&Line[itemLen],' ',WIDTH-itemLen);
        // Add NULL
		Line[WIDTH]='\0';
		if (iY == CurrMenu->GetCurrItemIndex()) {
            // Draw selected inverted
			u8x8.inverse();
			u8x8.drawString(X,iY+2,Line);
			u8x8.noInverse();
		}
		else {
            // Draw normal
			u8x8.drawString(X,iY+2,Line);
		}
	}
}

void Debug(const String& msg)
{
    Serial.println(msg);
}
