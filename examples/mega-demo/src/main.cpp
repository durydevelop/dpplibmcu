#include "Arduino.h"
#include <string>
#include <dmenu>
#include <ddigitalbutton>
#include <dservo>
#include <ddcmotor>
#include <oled.h>

#define COLS 16
#define ROWS 8

#define STR_MAIN_MENU       "Main Menu"
#define ID_MAIN_MENU        0

#define STR_SERVO_MAIN      "Servo Test"
#define ID_SERVO_MAIN       10
#define STR_SERVO_SETUP     "Servo Setup"
#define ID_SERVO_SETUP      11
#define STR_SERVO_RUN       "Servo Run"
#define ID_SERVO_RUN        12

#define STR_DCMOTOR_MAIN    "DC Motor Test"
#define ID_DCMOTOR_MAIN     20

#define STR_STEPPER_MAIN    "Stepper Test"
#define ID_STEPPER_MAIN     30

#define STR_START           "Start"
#define ID_START            40
#define STR_SETUP           "Setup"
#define ID_SETUP            41
#define STR_BACK            "Back"
#define ID_BACK             42

#define STR_PIN_UP          "Inc pin nr"
#define ID_PIN_UP           43
#define STR_PIN_DOWN        "Dec pin nr"
#define ID_PIN_DOWN         44
//#define STR_GO              "Run Test"
//#define ID_GO               45
//#define STR_STOP            "Stop Test"
//#define ID_STOP             46
//#define STR_STEP            "Step Test"
//#define ID_STEP             47


DMenu* MenuMain(void);
DMenu* MenuMotor(std::string Title, int ID);
DMenu* MenuServoSetup(void);
DMenu* MenuDCMotorSetup(void);

void PrintMenu(DMenu *Menu, uint8_t Col = 0, uint8_t Row = 0);
void Debug(const String& msg);

void ServoRunTest(void);
void DCMotorRunTest(void);

OLED display(18,19,NO_RESET_PIN,OLED::W_128,OLED::H_64,OLED::CTRL_SSD1306,0x3C);

// Button
DDigitalButton *Button=nullptr;

// Servo
uint8_t ServoPin=PB1;
DServo *Servo=nullptr;

// DC Motor
uint8_t DCMotorPin1=PB2;
uint8_t DCMotorPin2=PB3;
DDCMotor *DCMotor=nullptr;

// Menu
// N.B. Only one DMenu instance is mantained in memory by CurrMenu pointer
// because before creating a new menu, current one is deleted.
// In this way memory will be preserved from unused menu that ramains allocated.
DMenu *CurrMenu=nullptr;

void setup()
{
    // Begin display
	display.begin();

    // Begin serial debug
	Serial.begin(9600);
	//Debug("Start");

    // Create Button
	Button=new DDigitalButton(6);
	//Debug("Button on input pin 6");

    // Create MainMenu
	CurrMenu=MenuMain();
    PrintMenu(CurrMenu);
}

// Poll Button and handle read status
void loop()
{
    DMenu *SelItem;

	switch (Button->Read()) {
		case DDigitalButton::PRESSED:
			CurrMenu->Down();
            PrintMenu(CurrMenu);
			break;
		case DDigitalButton::LONG_PRESSED: {
			SelItem=CurrMenu->Select();
            int ItemID=SelItem->GetID();
            //Debug("SELECT "+String(ItemID));
            if (ItemID == ID_SERVO_MAIN) {
                CurrMenu=MenuMotor(STR_SERVO_MAIN,ID_SERVO_MAIN);
                PrintMenu(CurrMenu);
            }
            else if (ItemID == ID_DCMOTOR_MAIN) {
                CurrMenu=MenuMotor(STR_DCMOTOR_MAIN,ID_DCMOTOR_MAIN);
                PrintMenu(CurrMenu);
            }
            else if (ItemID == ID_STEPPER_MAIN) {
                //Debug("ID_STEPPER_MAIN");
                CurrMenu=MenuMotor(STR_STEPPER_MAIN,ID_STEPPER_MAIN);
                PrintMenu(CurrMenu);
            }
            else if (ItemID == ID_SETUP) {
                //Debug("ID_SETUP");
                if (CurrMenu->GetID() == ID_SERVO_MAIN) {
                    // Servo setup screen
                    CurrMenu=MenuServoSetup();
                    PrintMenu(CurrMenu);
                }
                else if (CurrMenu->GetID() == ID_DCMOTOR_MAIN) {
                    // DCMotor setup screen
                    CurrMenu=MenuDCMotorSetup();
                    PrintMenu(CurrMenu);
                }
                else if (CurrMenu->GetID() == ID_STEPPER_MAIN) {
                    // DStepper setup screen

                }
            }
            else if (ItemID == ID_START) {
                //Debug("ID_START");
                if (CurrMenu->GetID() == ID_SERVO_MAIN) {
                    // Create DServo object
                    Servo=new DServo();
                    ServoRunTest();
                }
                else if (CurrMenu->GetID() == ID_DCMOTOR_MAIN) {
                    // Create DDCMotor object
                    DCMotor=new DDCMotor(DCMotorPin1,DCMotorPin2);
                    DCMotorRunTest();
                }
            }
            else if (ItemID == ID_PIN_UP) {
                //Debug("ID_PIN_UP");
                if (CurrMenu->GetID() == ID_SERVO_SETUP) {
                    // Servo inc pin nr
                    ServoPin++;
                    PrintMenu(CurrMenu);
                }
            }
            else if (ItemID == ID_PIN_DOWN) {
                //Debug("ID_PIN_DOWN");
                if (CurrMenu->GetID() == ID_SERVO_SETUP) {
                    // Servo inc pin nr
                    ServoPin--;
                    PrintMenu(CurrMenu);
                }
            }
            else if (ItemID == ID_BACK) {
                //Debug("ID_BACK");
                // Handle back using GetID()
                // N.B. Don't use GetParent() because only one DMenu instance
                // is active in memory by CurrMenu pointer
                switch (CurrMenu->GetID()) {
                    case ID_SERVO_MAIN:
                        // Free DServo object
                        delete Servo;
                        Servo=nullptr;
                    case ID_DCMOTOR_MAIN:
                        // Free DDCMotor object
                        delete DCMotor;
                        DCMotor=nullptr;
                    case ID_STEPPER_MAIN:
                        //Debug("...to MenuMain()");
                        CurrMenu=MenuMain();
                        break;
                    case ID_SERVO_SETUP:
                        //Debug("...to ID_SERVO_MAIN");
                        CurrMenu=MenuMotor(STR_SERVO_MAIN,ID_SERVO_MAIN);
                        break;
                    case ID_SERVO_RUN:
                        CurrMenu=MenuMotor(STR_SERVO_MAIN,ID_SERVO_MAIN);
                        break;
                    default:
                        break;
                }
                PrintMenu(CurrMenu);
            }
			break;
        }
		default:
			break;
	}
}

DMenu* MenuMain(void) {
	if (CurrMenu) {
        // Only one menu must be in memory
		delete CurrMenu;
	}

	CurrMenu=new DMenu(STR_MAIN_MENU,ID_MAIN_MENU);
	CurrMenu->Loop=true;

	CurrMenu->AddItem(STR_SERVO_MAIN,ID_SERVO_MAIN,true);
    CurrMenu->AddItem(STR_DCMOTOR_MAIN,ID_DCMOTOR_MAIN);
	CurrMenu->AddItem(STR_STEPPER_MAIN,ID_STEPPER_MAIN);

    return(CurrMenu);
}

DMenu* MenuMotor(std::string Title, int ID) {
	if (CurrMenu) {
        // To avoid memory use, only one menu must be in memory
		delete CurrMenu;
	}

	CurrMenu=new DMenu(Title.c_str(),ID);
	CurrMenu->Loop=true;

	CurrMenu->AddItem(STR_START,ID_START,true);
    CurrMenu->AddItem(STR_SETUP,ID_SETUP);
	CurrMenu->AddItem(STR_BACK,ID_BACK);

    return(CurrMenu);
}

// ***************************************************************************************
// **************************************** Servo ****************************************
// ***************************************************************************************

DMenu* MenuServoSetup(void)
{
    if (CurrMenu) {
        // To avoid memory use, only one menu must be in memory
		delete CurrMenu;
	}

	CurrMenu=new DMenu(STR_SERVO_SETUP,ID_SERVO_SETUP);
	CurrMenu->Loop=true;

	CurrMenu->AddItem(STR_PIN_UP,ID_PIN_UP,true);
    CurrMenu->AddItem(STR_PIN_DOWN,ID_PIN_DOWN);
	CurrMenu->AddItem(STR_BACK,ID_BACK);

    return(CurrMenu);
}

void ServoRunTest(void)
{
    display.clear();
    display.printf_T(1,0,"Servo run on pin %d",ServoPin);
    display.printf_T(1,1,"Long press to quit");
    display.display();

    Servo->attach(ServoPin);
    if (!Servo->attached()) {
        display.printf_T(4,4,"Servo ERROR");
        return;
    }

    int uS=1000;
    int inc=500;
    int now=millis();
    do {
        if (millis()-now > 500) {
            Servo->writeMicroseconds(uS+=inc);
            if (uS >= 2000 || uS <= 1000) {
                inc=-inc;
            }
            display.printf_T(0,4,"Degrees    = %3d",Servo->GetCurrDegrees());
            display.printf_T(0,5,"Pulse usec = %4d",Servo->GetCurrPulseUs());
            display.display();
            now=millis();
        }
        
    }while(Button->Read() != DDigitalButton::PRESSED);
    
    Servo->detach();
}

// ***************************************************************************************
// *************************************** DCMotor ***************************************
// ***************************************************************************************

DMenu* MenuDCMotorSetup(void)
{
    if (CurrMenu) {
        // To avoid memory use, only one menu must be in memory
		delete CurrMenu;
	}

	CurrMenu=new DMenu(STR_DCMOTOR_SETUP,ID_DCMOTOR_SETUP);
	CurrMenu->Loop=true;

	CurrMenu->AddItem(STR_PIN_DIR,ID_PIN_DIR,true);
    CurrMenu->AddItem(STR_PIN_PWM,ID_PIN_PWM);
	CurrMenu->AddItem(STR_BACK,ID_BACK);

    return(CurrMenu);
}

DMenu* MenuDCMotorSetupPin(uint8_t Pin)
{
    if (CurrMenu) {
        // To avoid memory use, only one menu must be in memory
		delete CurrMenu;
	}

	CurrMenu=new DMenu(STR_DCMOTOR_SETUP+Pin,ID_DCMOTOR_SETUP+Pin);
	CurrMenu->Loop=true;

	CurrMenu->AddItem(STR_PIN_UP,ID_PIN_UP,true);
    CurrMenu->AddItem(STR_PIN_DOWN,ID_PIN_DOWN);
	CurrMenu->AddItem(STR_BACK,ID_BACK);

    return(CurrMenu);
}

void DCMotorRunTest(void)
{
    display.clear();
    display.printf_T(1,0,"DCMotor on pins %d - %d",DCMotorPin1,DCMotorPin2);
    display.printf_T(1,1,"Long press to quit");
    display.display();

    DCMotor->attach(DCMotorPin1,DCMotorPin2,DDCMotor::PWM_PWM);
    if (!DCMotor->attached()) {
        display.printf_T(4,4,"Servo ERROR");
        return;
    }

    uint8_t speed=0;
    int8_t inc=1;
    int now=millis();
    do {
        DCMotor->Set(speed);
        if (millis()-now > 500) {
            inc=-inc;
            display.printf_T(0,4,"Degrees    = %3d",Servo->GetCurrDegrees());
            display.printf_T(0,5,"Pulse usec = %4d",Servo->GetCurrPulseUs());
            display.display();
            now=millis();
        }
        
    }while(Button->Read() != DDigitalButton::PRESSED);
    
    Servo->detach();
}

// ***************************************************************************************
// ****************************************  ****************************************
// ***************************************************************************************

void PrintMenu(DMenu* Menu, uint8_t Col, uint8_t Row)
{
    display.clear();
    // Line buffer
	char Line[COLS+1];
    memset(Line,' ',COLS+1);

    // Create centered menu title line
    uint8_t nameLen=strlen(Menu->GetName());
    uint8_t startCol=(COLS-nameLen)/2;
	memcpy(&Line[startCol],Menu->GetName(),nameLen);
    Line[COLS]='\0';

    // Print menu title
    display.drawString(Col,Row,Line);

    // Starts from second line
	for (uint8_t ixRow=Row; ixRow<Menu->GetItemsCount();ixRow++) {
        DMenu* Item=CurrMenu->GetItem(ixRow);
        // Starts with item name 
        uint8_t itemLen=strlen(Item->GetName());
		memcpy(Line,Item->GetName(),itemLen);
        // Fill with spaces
		memset(&Line[itemLen],' ',COLS-itemLen);
        // Add NULL
		Line[COLS]='\0';
        std::string sLine=Item->GetName();
		if (ixRow == CurrMenu->GetCurrItemIndex()) {
            // Draw selected
            display.inverse();
            //display.drawString(X,iY+2,">");
            display.drawString(Col,ixRow+2,sLine.c_str());
            display.noInverse();
		}
		else {
            // Draw not selected
			//display.drawString(X,iY+2," ");
            display.drawString(Col,ixRow+2,sLine.c_str());
		}
        //display.drawString(X+2,iY+2,Line);
	}

    if (Menu->GetID() == ID_SERVO_SETUP) {
        display.printf_T(2,7,"Servo Pin = %d",ServoPin);
    }
    else if (Menu->GetID() == ID_SERVO_RUN) {
        display.printf_T(2,7,"Pin = %d Pos = %d°",ServoPin,Servo->read());
    }

    display.display();  // **** only for OLED ****
}

void Debug(const String& msg)
{
    Serial.println(msg);
}