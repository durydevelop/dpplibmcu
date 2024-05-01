/*
 * digitalOuput.cpp
 *
 *  Created on: 10/mag/2013
 *      Author: dury
 */

/** \mainpage Classe DDigitalOutput per Arduino
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
DDigitalOutput::DDigitalOutput(int digitalPin, DGpioHandle gpioHandle)
{
    pin=digitalPin;
    currLevel=LOW;
    lastResult=DERR_CLASS_NOT_BEGUN;

    //std::cout << "DDigitalOutput() digitalPin=" << digitalPin<< " gpioHandle=" << gpioHandle << std::endl;

    if (gpioHandle < 0) {
        // Try init on first device
        //std::cout << "auto set handle" << std::endl;
        lastResult=initGpio(0,handle);
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
 * @brief Destroy the DDigitalOutput::DDigitalOutput object and free pin use.
 */
DDigitalOutput::~DDigitalOutput()
{
    releasePin(pin,handle);
}

bool DDigitalOutput::begin(int initialLevel)
{
    if (handle >= 0) {
        lastResult=initPin(pin,DPinMode::OUTPUT,DPinFlags::NO_FLAGS,handle);
        if (lastResult == DRES_OK) {
            // Set initial level
            write(initialLevel);
            // Read current input state
            //currLevel=read();
            currLevel=initialLevel;
            return true;
        }
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
    currLevel=readPin(pin,handle);
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
    lastResult=writePin(pin,level,handle);
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

std::string DDigitalOutput::getLastError(void)
{
    return getErrorCode(lastResult);
}