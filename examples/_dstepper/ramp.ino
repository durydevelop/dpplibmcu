#include <Arduino.h>
#include <dstepper.h>

DStepper dStepper(12,13);

void setup() {
    dStepper.begin();
    dStepper.On();
}

void loop() {
    dStepper.RampRpm(4000,120,DStepper::ROT_CC);
    delay(4000);
    dStepper.RampRpm(4000,0,DStepper::ROT_CC);
    delay(2000);
}