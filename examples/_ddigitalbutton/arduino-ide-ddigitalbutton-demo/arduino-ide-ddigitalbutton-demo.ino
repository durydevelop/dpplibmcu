#include "Arduino.h"
#include "DDigitalButton.h"

DDigitalButton Button(6);

void setup()
{
  // Begin serial debug
	Serial.begin(9600);
	Serial.println("Start");
}

void loop()
{
  switch (Button.Read()) {
	  case DDigitalButton::PRESSED:
      Serial.print("\r\n<PRESSED>\r\n");
      break;
    case DDigitalButton::LONG_PRESSED:
      Serial.print("\r\n<LONG_PRESSED>\r\n");
      break;
    case DDigitalButton::DBL_PRESSED:
      Serial.print("\r\n<DBL_PRESSED>\r\n");
      break;
    case DDigitalButton::RELEASED:
      Serial.print("\r\n<RELEASED>\r\n");
      break;
    case DDigitalButton::DBL_PRESSING:
      Serial.print("DBL_PRESSING ");
      break;
    case DDigitalButton::PRESS:
      Serial.print("PRESS ");
      break;
    case DDigitalButton::LONG_PRESSING:
      Serial.print("LONG_PRESSING ");
      break;
    case DDigitalButton::RELEASE:
      //Serial.print("RELEASE ");
      break;
    default:
      break;
	}
}
