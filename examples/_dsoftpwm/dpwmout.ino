#include <Arduino.h>
#include <dsoftpwm>

DPwmOut Pwm();

void setup() {

  Pwm.begin();
  Pwm.SetFreq(400,50);
  Pwm.On();
  
pinMode(4,OUTPUT);
}

void loop() {
    digitalWrite(4,HIGH);
    delay(500);
    digitalWrite(4,LOW);
    delay(500);
}
