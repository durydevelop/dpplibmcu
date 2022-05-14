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

#include "DDigitalOutput.h"

//! Costruttore 1
/**
 * @param DigitalPin	->	pin da utilizzare come output
 */
DDigitalOutput::DDigitalOutput(uint8_t DigitalPin)
{
	Pin=DigitalPin;
	Attached=InitPin();
	CurrState=Read();
}

//! Costruttore 2
/**
 * @param DigitalPin	->	pin da utilizzare come output
 * @param InitialState	-> Stato logico iniziale da attivare: [HIGH,LOW] oppure [1,0]
 */
DDigitalOutput::DDigitalOutput(uint8_t DigitalPin, uint8_t State)
{
	Pin=DigitalPin;
	Attached=InitPin();
	Write(State);
	CurrState=Read();
}

//! Porta il pin di uscita a livello logico 1 (5V)
void DDigitalOutput::High(void)
{
	Write(HIGH);
}

//! Imposta il pin di uscita a livello logico 0 (GND)
void DDigitalOutput::Low(void)
{
	Write(LOW);
}

//! Cambia lo stato del pin di uscita
void DDigitalOutput::Toggle(void)
{
    Write(!CurrState);
}

//! overload operatore ==
DDigitalOutput::operator bool()
{
	return Read();
}

//! overload operatore =
DDigitalOutput& DDigitalOutput::operator= (bool State)
{
	Write(State);
	return *this;
}

bool DDigitalOutput::InitPin(void)
{
    #ifdef ARDUINO
        pinMode(Pin,OUTPUT);
    #else
        #ifdef PIGPIO_VERSION
			if (gpioInitialise() == PI_INIT_FAILED) {
            	return false;
        	}
            gpioSetMode(Pin,PI_OUTPUT);
        #else
            pinMode(Pin,OUTPUT);
        #endif
    #endif
	return true;
}

//! Legge lo stato del pin
/**
 * @return lo stato logico dell'ingresso: HIGH o LOW
 * N.B. Memorizza lo stato in pinState.
 */
short int DDigitalOutput::Read(void) {
    if (!Attached) return(-1);
    #ifdef ARDUINO
        CurrState=digitalRead(Pin);
    #else
        #ifdef PIGPIO_VERSION
            CurrState=gpioRead(Pin);
        #else
            CurrState=digitalRead(Pin);
        #endif
    #endif

    return(CurrState);
}

//! Uso interno: Setta il pin di uscita al livello logico richiesto
/**
 * @param State	->	Il livello logico da impostare [HIGH,LOW] oppure [1,0]
 */
short int DDigitalOutput::Write(uint8_t State)
{
    if (!Attached) return(-1);
    #ifdef ARDUINO
        digitalWrite(Pin,State);
    #else
        #ifdef PIGPIO_VERSION
            gpioWrite(Pin,State);
        #else
            digitalWrite(Pin,State);
        #endif
    #endif

	CurrState=State;
    return(CurrState);
}

/**
 * @return input pin number.
 */
uint8_t DDigitalOutput::GetPin(void)
{
	return(Pin);
}

bool DDigitalOutput::attached(void)
{
    return(Attached);
}
