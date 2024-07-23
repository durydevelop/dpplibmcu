/**
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

/**
 * @param digitalPin	->	pin da utilizzare come input
 * @param pullUp		->	se true viene attivata la resistena interna di pull-up
 */
DDigitalInput::DDigitalInput(int digitalPin, DGpioHandle gpioHandle)
{
	pin=digitalPin;
    currLevel=LOW;
    prevLevel=currLevel;
    lastResult=DERR_CLASS_NOT_BEGUN;

    //std::cout << "DDigitalInput() digitalPin=" << digitalPin<< " gpioHandle=" << gpioHandle << std::endl;

    if (gpioHandle < 0) {
        // Try init on first device
        //std::cout << "auto set handle" << std::endl;
        lastResult=initGpio(0,handle);
        if (lastResult > 0) {
            lastResult=DERR_CLASS_NOT_BEGUN;
        }
    }
    else {
        if (isGpioReady(gpioHandle)) {
            handle=gpioHandle;
        }
        else {
            lastResult=DERR_GPIO_NOT_READY;
        }
    }
}

/**
 * @brief Destroy the DDigitalInput::DDigitalInput object and free pin use.
 */
DDigitalInput::~DDigitalInput()
{
    releasePin(pin,handle);
}

bool DDigitalInput::begin(bool pullUp, unsigned int msecDebounce)
{
    if (handle >= 0) {
        lastResult=initPin(pin,pullUp ? INPUT_PULLUP : INPUT,DPinFlags::NO_FLAGS,handle);
        if (lastResult == DRES_OK) {
            // Read current input state
            read();
        }
        prevLevel=currLevel;
    }

	// Debounce msec
	debounceMsec=msecDebounce;
	prevMsec=millis();
    return lastResult == DRES_OK;
}

/**
 * @brief 
 * 
 * @param NewLevel	->	Puntatore alla variabile che conterrà il nuovo stato (se null viene ignorata)
 * @return
 * true		se cambiato
 * false	se non cambiato
 */
bool DDigitalInput::isChanged(short int *newLevel)
{
	bool changed=false;
	read();

    if (currLevel != prevLevel) {
        if (debounceMsec > 0) {
            currMsec=millis();
            if ((currMsec-prevMsec) >= debounceMsec) {
                prevMsec=currMsec;
		        changed=true;
            }
        }
        else {
            changed=true;
        }
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
	read();
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
	read();
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
int DDigitalInput::read(void) {
    currLevel=readPin(pin,handle);
    if (currLevel < 0) {
        lastResult=currLevel;
    }
    else {
        lastResult=DRES_OK;
    }
    return currLevel;
}

/**
 * @return input pin number.
 */
int DDigitalInput::getPin(void)
{
	return(pin);
}

bool DDigitalInput::isAttached(void)
{
    return lastResult == DRES_OK;
}

std::string DDigitalInput::getLastError(void)
{
    return getErrorCode(lastResult);
}
