#include <Arduino.h>
#include <dstepper.h>

DStepper dStepper(0,2);

void setup() {
    dStepper.begin();
    dStepper.On();
}

unsigned long now=millis();
int f=1;
int inc=1;
void loop() {
    dStepper.Ramp(120,4000);
    delay(4000);
    dStepper.Ramp(0,4000);
}