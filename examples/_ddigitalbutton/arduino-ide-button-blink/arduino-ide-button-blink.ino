/**
 * DDigitalButton class example.
 * N.B. If You don't have installed ddigialio lib, You need to copy DDigitalButton.h and DDigitalButton.cpp for src folder
 * to this folder.
 */
#include "DDigitalButton.h"

DDigitalButton Button(6,    // pin
                      LOW,  // pin to LOW level = button is pressed
                      true, // Internal pull-up resistor is enabled
                      150,  // debounce millis time for PRESS detect
                      1000  // long press millis detect time
                      );
int Level=LOW;

void setup() {
  Serial.begin(9600);
  Serial.println("Start");
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Button on pin 6");
  Serial.println("If You press (and release) the button, on board led toggle his status.");
  Serial.println("If You double-press the button, on board led blink 2 times.");
  Serial.println("If You long-press (1 second) the button, on board led switch on for 1 second.");
}

void loop() {
  switch (Button) {
    case DDigitalButton::PRESSED:
      Serial.println("PRESSED");
      Level=!Level;
      digitalWrite(LED_BUILTIN,Level);
      break;
    case DDigitalButton::LONG_PRESSED:
      Serial.println("LONG_PRESSED");
      digitalWrite(LED_BUILTIN,LOW);
      delay(100);
      digitalWrite(LED_BUILTIN,HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN,LOW);
      delay(100);
      break;
    case DDigitalButton::DBL_PRESSED:
      Serial.println("DBL_PRESSED");
      for (uint8_t nTimes=0; nTimes<4; nTimes++) {
        digitalWrite(LED_BUILTIN,HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN,LOW);
        delay(100);
      }
      break;
  }
}
