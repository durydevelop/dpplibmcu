/*! \mainpage Classe DDigitalInput per Arduino
*
* \section intro_sec Introduzione
*
* Permette di gestire pi&ugrave; facilmente un input pin:
* - Usare l'operatore '=='
* - Rilevare un cambio di stato
* - Gestire l'antirimbalzo
*
* \section usage_sec Utilizzo
*
* @code
*
DDigitalInput inTest(8,true); // pulsante collegato tra il pin 8 e gnd (attiva anche la resistenza di pull-up)
pinMode(13, OUTPUT); // Led Pin
void setup()
{
  	Serial.begin(9600);
}

void loop()
{
	if (inTest == LOW) {
 		digitalWrite(13,HIGH); accende il led
 	}
 	else {
 		digitalWrite(13,LOW); spegne il led
 	}

	if (inTest.isChanged()) {
		Serial.print("Stato cambiato in ");
		Serial.println(inTest);
	}
}
* @endcode
*/

#include <dpplib-gpio>

#include "DDigitalInput.h"

/**
 * @param digitalPin	->	pin da utilizzare come input
 * @param pullUp		->	se true viene attivata la resistena interna di pull-up
 */
DDigitalInput::DDigitalInput(int DigitalPin, bool PullUp, unsigned int MsecDebounce)
{
	Pin=DigitalPin;
	Attached=InitPin(DigitalPin,PullUp ? INPUT_PULLUP_MODE : INPUT_MODE);

	// Read current input state
	CurrLevel=Read();
	PrevLevel=CurrLevel;

	// Debounce msec
	DebounceMsec=MsecDebounce;
	PrevMsec=Now();

}

/**
 * @brief 
 * 
 * @param NewLevel	->	Puntatore alla variabile che conterr il nuovo stato (se null virnr ignorata)
 * @return
 * true		se &egrave; cambiato
 * false	se &egrave; non &egrave; cambiato
 */
bool DDigitalInput::IsChanged(short int *NewLevel)
{
	bool Changed=false;
	CurrLevel=Read();
	CurrMsec=Now();
	if (CurrLevel != PrevLevel && (CurrMsec-PrevMsec) > DebounceMsec) {
		PrevMsec=CurrMsec;
		Changed=true;
	}

	PrevLevel=CurrLevel;
	if (NewLevel != nullptr) {
		*NewLevel=CurrLevel;
	}
	return Changed;
}

//! Verifica se lo stato del pin &egrave; cambiato da HIGH in LOW rispetto all'ultimo controllo
/**
 * @return
 * true		se &egrave; cambiato in LOW
 * false	se &egrave; non &egrave; cambiato
 */
bool DDigitalInput::IsChangedToLow(void)
{
	bool ret=false;
	CurrLevel=Read();
	CurrMsec=Now();
	if (CurrLevel != PrevLevel && (CurrMsec-PrevMsec) > DebounceMsec) {
		PrevMsec=CurrMsec;
		if (CurrLevel == LOW) {
			ret=true;
		}
	}

	PrevLevel=CurrLevel;
	return ret;
}

//! Verifica se lo stato del pin &egrave; cambiato da LOW in HIGH rispetto all'ultimo controllo
/**
 * @return
 * true		se &egrave; cambiato in HIGH
 * false	se &egrave; non &egrave; cambiato
 */
bool DDigitalInput::IsChangedToHigh(void)
{
	bool ret=false;
	CurrLevel=Read();
	CurrMsec=Now();
	if (CurrLevel != PrevLevel && (CurrMsec-PrevMsec) > DebounceMsec) {
		PrevMsec=CurrMsec;
		if (CurrLevel == HIGH) {
			ret=true;
		}
	}

	PrevLevel=CurrLevel;
	return ret;
}

//! overload operatore ==
DDigitalInput::operator int()
{
	return Read();
}

/*
bool DDigitalInput::InitPin(bool PullUp)
{
    #ifdef ARDUINO
		if (PullUp) {
			pinMode(Pin,INPUT_PULLUP);
		}
		else {
			pinMode(Pin,INPUT);
		}
        
    #else
        #ifdef PIGPIO_VERSION
			if (gpioInitialise() == PI_INIT_FAILED) {
            	return false;
        	}
            gpioSetMode(Pin,PI_INPUT);
			if (PullUp) {
				SetPullUp();
			}
        #else
            if (PullUp) {
				pinMode(Pin,IMPUT_PULLUP);
			}
			else {
				pinMode(Pin,INPUT);
			}
        #endif
    #endif
	return true;
}
*/

//! Legge lo stato del pin
/**
 * @return lo stato logico dell'ingresso: HIGH o LOW
 * N.B. Memorizza lo stato in pinState.
 */
short int DDigitalInput::Read(void) {
	if (!Attached) return(-1);
    CurrLevel=ReadPin(Pin);
    return(CurrLevel);
}

/*
void DDigitalInput::SetPullUp(void) {
	if (!Attached) return;
    #ifdef ARDUINO
        digitalRead(Pin);
    #else
        #ifdef PIGPIO_VERSION
            gpioSetPullUpDown(Pin,PI_PUD_UP);
        #else
            digitalWrite(Pin,HIGH);
        #endif
    #endif
}

#ifdef PIGPIO_VERSION
	void DDigitalInput::SetPullDown(void) {
		if (!Attached) return;
		gpioSetPullUpDown(Pin,PI_PUD_DOWN);
	}

	void DDigitalInput::SetFloat(void) {
		if (!Attached) return;
		gpioSetPullUpDown(Pin,PI_PUD_OFF);
	}
#endif
*/
/**
 * @return input pin number.
 */
short int DDigitalInput::GetPin(void)
{
	return(Pin);
}

bool DDigitalInput::attached(void)
{
    return(Attached);
}
