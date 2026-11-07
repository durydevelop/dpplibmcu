/*
 * digitalOuput.cpp
 *
 *  Created on: 10/mag/2013
 *      Author: dury
 */

/**
*
* \section intro_sec Introduzione
*
* Permette di gestire pi&ugrave; facilmente un output pin:
* - Cambio di stato usando l'operatore '='
* - Cambio veloce di stato con la funziona Toggle()
* - Memorizza lo stato corrente.
* - Leggere lo stato con l'operatore '=='
*
* \section usage_sec Utilizzo
*
* @code
*
// Lampeggia il led collegato al pin 13
DDigitalOutput led(13,LOW); // Led Pin, stato iniziale LOW
void setup()
{
  	Serial.begin(9600);
}

void loop()
{
	led.Toggle();
	delay(1000);
	Serial.print("Stato cambiato in ");
	Serial.println(led);
}
* @endcode
*/

#include "ddigitaloutput.h"

/**
 * @param DigitalPin	->	pin da utilizzare come output
 * @param InitialState	-> Stato logico iniziale da attivare: [HIGH,LOW] oppure [1,0]
 */
#ifndef ARDUINO
DDigitalOutput::DDigitalOutput(int digitalPin, DGpioHandle gpioHandle)
#else
DDigitalOutput::DDigitalOutput(int digitalPin)
#endif
{
    pin=digitalPin;
    currLevel=LOW;
    lastResult=DERR_CLASS_NOT_BEGUN;

    //std::cout << "DDigitalOutput() digitalPin=" << digitalPin<< " gpioHandle=" << gpioHandle << std::endl;
#ifndef ARDUINO
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
#endif
}

/**
 * @brief Destroy the DDigitalOutput::DDigitalOutput object and free pin use.
 */
#ifndef ARDUINO
DDigitalOutput::~DDigitalOutput()
{
    releasePin(pin,handle);
}
#endif

bool DDigitalOutput::begin(int initialLevel)
{
    #ifndef ARDUINO
        if (handle >= 0) {
            lastResult=initPin(pin,DPinMode::PIN_MODE_OUTPUT,DPinFlags::PIN_FLAG_NONE,handle);
        }
    #else
        lastResult=initPin(pin,DPinMode::PIN_MODE_OUTPUT);
    #endif
    
    if (lastResult == DRES_OK) {
        // Set initial level
        write(initialLevel);
        // Read current input state
        //currLevel=read();
        currLevel=initialLevel;
        return true;
    }
    
    return false;
}

//! Porta il pin di uscita a livello logico 1 (5V)
void DDigitalOutput::high(void)
{
	write(HIGH);
}

//! Imposta il pin di uscita a livello logico 0 (GND)
void DDigitalOutput::low(void)
{
	write(LOW);
}

//! Cambia lo stato del pin di uscita
void DDigitalOutput::toggle(void)
{
    write(!currLevel);
}

//! overload operatore ==
DDigitalOutput::operator int()
{
	return read();
}

//! overload operatore =
DDigitalOutput& DDigitalOutput::operator= (bool level)
{
	write(level);
	return *this;
}

//! Legge lo stato del pin
/**
 * @return lo stato logico dell'ingresso: HIGH o LOW (negative value is error code).
 * N.B. aggiorna currLevel.
 */
int DDigitalOutput::read(void) {
    #ifndef ARDUINO
        currLevel=readPin(pin,handle);
    #else
        currLevel=readPin(pin);
    #endif
    
    if (currLevel < 0) {
        lastResult=currLevel;
    }
    else {
        lastResult=DRES_OK;
    }
    return currLevel;
}

//! Uso interno: Setta il pin di uscita al livello logico richiesto
/**
 * @param State	->	Il livello logico da impostare [HIGH,LOW] oppure [1,0]
 */
int DDigitalOutput::write(int level)
{
    #ifndef ARDUINO
        lastResult=writePin(pin,level,handle);
    #else
        lastResult=writePin(pin,level);
    #endif

	currLevel=level;
    return lastResult;
}

/**
 * @return input pin number.
 */
int DDigitalOutput::getPin(void)
{
	return(pin);
}

bool DDigitalOutput::isAttached(void)
{
    return lastResult == DRES_OK;
}

#ifndef ARDUINO
std::string DDigitalOutput::getLastError(void)
{
    //return std::string(lguErrorText(lastResult));
    return getErrorCode(lastResult);
}
#endif