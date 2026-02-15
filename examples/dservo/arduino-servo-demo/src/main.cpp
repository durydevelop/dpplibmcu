#include <Arduino.h>
#include <dservo>

DServo *Servo;

void setup() {
    delay(1000);
    Servo=new DServo();
    delay(1000);
    Servo->attach(2);
}

void loop() {

  Servo->writeMicroseconds(1000);
  delay(1000);
  Servo->writeMicroseconds(1500);
  delay(1000);
  Servo->writeMicroseconds(2000);
  delay(1000);

}