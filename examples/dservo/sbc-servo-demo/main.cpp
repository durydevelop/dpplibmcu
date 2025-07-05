#include <iostream>
#include <dservo>
#include <filesystem>

void showUsage(std::filesystem::path binaryName)
{
    std::cout <<
        "This program move a servo to minimun position (1 ms pulse), wait 1 sec, move to middle position (1.5 ms pulse), wait 1 sec, move to maximun position (2 ms pulse), wait 1 sec, restart from minimum." << std::endl <<
        "Usage: " << binaryName.stem().string() << " <INPUT pin> <OUTPUT pin>" << std::endl <<
        "    <INPUT pin>    GPIO pin number used for INPUT." << std::endl <<
        "    <OUTPUT pin>   GPIO pin number used for OUTPUT." << std::endl <<
        "    -h, --help     Show this help" << std::endl <<
        "Example:" << std::endl <<
        binaryName.stem().string() << " 4 5" << std::endl <<
        "will out on GPIO5 the GPIO4 level when changes." << std::endl;
}

int main(int argc, char** argv) {

    int servoPin=0;

    if (argc > 1) {
        std::string sArg(argv[1]);
        if (sArg == "-h" || sArg == "--help") {
            showUsage(argv[0]);
            exit(1);
        }
    
        if (argc >= 2) {
            servoPin=atoi(argv[1]);
        }
    }
    else {
        std::cout << "Missing argument." << std::endl;
        showUsage(argv[0]);
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
