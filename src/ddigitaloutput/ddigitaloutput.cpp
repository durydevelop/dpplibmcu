/*
 * digitalOuput.cpp
 *
 *  Created on: 10/mag/2013
 *      Author: dury
 */

/*! \mainpage Classe DDigitalOutput per Arduino
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

//! Costruttore 1
/**
 * @param DigitalPin	->	pin da utilizzare come output
 */
DDigitalOutput::DDigitalOutput(uint8_t digitalPin)
{
	pin=digitalPin;
	gpioAttached=initPin(digitalPin,DPinMode::OUTPUT);
	currLevel=read();
}

//! Costruttore 2
/**
 * @param DigitalPin	->	pin da utilizzare come output
 * @param InitialState	-> Stato logico iniziale da attivare: [HIGH,LOW] oppure [1,0]
 */
DDigitalOutput::DDigitalOutput(uint8_t digitalPin, uint8_t initalLevel)
{
	pin=digitalPin;
	gpioAttached=initPin(digitalPin,DPinMode::OUTPUT);
	write(initalLevel);
	currLevel=read();
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
DDigitalOutput::operator bool()
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
 * @return lo stato logico dell'ingresso: HIGH o LOW
 * N.B. aggiorna currLevel.
 */
short int DDigitalOutput::read(void) {
    if (!gpioAttached) return -1;
    currLevel=readPin(pin);
    return currLevel;
}

//! Uso interno: Setta il pin di uscita al livello logico richiesto
/**
 * @param State	->	Il livello logico da impostare [HIGH,LOW] oppure [1,0]
 */
short int DDigitalOutput::write(uint8_t level)
{
    if (!gpioAttached) return -1;
    writePin(pin,level);

	currLevel=level;
    return currLevel;
}

/**
 * @return input pin number.
 */
uint8_t DDigitalOutput::getPin(void)
{
	return(pin);
}

bool DDigitalOutput::isAttached(void)
{
    return(gpioAttached);
}
