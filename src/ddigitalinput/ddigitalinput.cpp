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

#include "ddigitalinput.h"
#include <dgpio>

/**
 * @param digitalPin	->	pin da utilizzare come input
 * @param pullUp		->	se true viene attivata la resistena interna di pull-up
 */
DDigitalInput::DDigitalInput(int digitalPin, bool pullUp, unsigned int msecDebounce)
{
	pin=digitalPin;
	gpioAttached=initPin(digitalPin,pullUp ? INPUT_PULLUP : INPUT);

	// Read current input state
	currLevel=read();
	prevLevel=currLevel;

	// Debounce msec
	debounceMsec=msecDebounce;
	prevMsec=millis();
}

/**
 * @brief 
 * 
 * @param NewLevel	->	Puntatore alla variabile che conterrà il nuovo stato (se null virnr ignorata)
 * @return
 * true		se cambiato
 * false	se non cambiato
 */
bool DDigitalInput::isChanged(short int *newLevel)
{
	bool changed=false;
	currLevel=read();
	currMsec=millis();
	if (currLevel != prevLevel && (currMsec-prevMsec) > debounceMsec) {
		prevMsec=currMsec;
		changed=true;
	}

	prevLevel=currLevel;
	if (newLevel != nullptr) {
		*newLevel=currLevel;
	}
	return changed;
}

//! Verifica se lo stato del pin &egrave; cambiato da HIGH in LOW rispetto all'ultimo controllo
/**
 * @return
 * true		se &egrave; cambiato in LOW
 * false	se &egrave; non &egrave; cambiato
 */
bool DDigitalInput::isChangedToLow(void)
{
	bool ret=false;
	currLevel=read();
	currMsec=millis();
	if (currLevel != prevLevel && (currMsec-prevMsec) > debounceMsec) {
		prevMsec=currMsec;
		if (currLevel == LOW) {
			ret=true;
		}
	}

	prevLevel=currLevel;
	return ret;
}

//! Verifica se lo stato del pin &egrave; cambiato da LOW in HIGH rispetto all'ultimo controllo
/**
 * @return
 * true		se &egrave; cambiato in HIGH
 * false	se &egrave; non &egrave; cambiato
 */
bool DDigitalInput::isChangedToHigh(void)
{
	bool ret=false;
	currLevel=read();
	currMsec=millis();
	if (currLevel != prevLevel && (currMsec-prevMsec) > debounceMsec) {
		prevMsec=currMsec;
		if (currLevel == HIGH) {
			ret=true;
		}
	}

	prevLevel=currLevel;
	return ret;
}

//! overload operatore ==
DDigitalInput::operator int()
{
	return read();
}

//! Legge lo stato del pin
/**
 * @return lo stato logico dell'ingresso: HIGH o LOW
 * N.B. aggiorna currLevel.
 */
short int DDigitalInput::read(void) {
	if (!gpioAttached) return(-1);
    currLevel=readPin(pin);
    return(currLevel);
}

/**
 * @return input pin number.
 */
short int DDigitalInput::getPin(void)
{
	return(pin);
}

bool DDigitalInput::isAttached(void)
{
    return(gpioAttached);
}
