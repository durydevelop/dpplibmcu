#include <iostream>
#include <dservo>

int main(int argc, char** argv) {

    int servoPin=0;
    if (argc == 2) {
        servoPin=atoi(argv[1]);
    }
    else {
        std::cout << "Missing Pin argument, must specify servo pin nr" << std::endl;
        exit(1);
    }

    if (servoPin <= 0 || servoPin > 32) {
        std::cout << "Servo pin nr not valid." << std::endl;
        exit(1);
    }

    DGpioChip chip(0);
    if (!chip.isReady()) {
        std::cout << "DGpioChip not ready: " << chip.getLastError() << std::endl;
    }
    else {
        std::cout << chip.getInfo() << std::endl;
    }

    DServo servo(19,chip.handle());
    servo.begin();
    
    std::cout << "Start servo test on pin " << servoPin << std::endl;
    std::cout << "Press CTRL+C to stop" << std::endl;

    do {
        servo.writeMicroseconds(1000);
        std::cout << "Servo curr us " << servo.getCurrPulseUs() << std::endl;
        delay(1000);
        servo.writeMicroseconds(1500);
        std::cout << "Servo curr us " << servo.getCurrPulseUs() << std::endl;
        delay(1000);
        servo.writeMicroseconds(2000);
        std::cout << "Servo curr us " << servo.getCurrPulseUs() << std::endl;
        delay(1000);
    }while(true);

    return 0;
}
