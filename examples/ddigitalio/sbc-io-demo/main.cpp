#include <iostream>
#include <ddigitalinput>
#include <ddigitaloutput>

int main(int argc, char** argv) {

    int inPin=0;
    int outPin=19;
    if (argc == 3) {
        inPin=atoi(argv[1]);
        outPin=atoi(argv[2]);
    }
    else {
        std::cout << "Missing Pin argument, must specify input and output pin" << std::endl;
        exit(1);
    }

    if (inPin <= 0 || inPin > 32) {
        std::cout << "Input pin nr not valid." << std::endl;
        exit(1);
    }

    if (outPin <= 0 || outPin > 32) {
        std::cout << "Output pin nr not valid." << std::endl;
        exit(1);
    }

    //lguSetInternal(LG_CFG_ID_DEBUG_LEVEL,12);
    DGpioChip chip(0);
    if (!chip.isReady()) {
        std::cout << "DGpioChip not ready: " << chip.getLastError() << std::endl;
    }
    else {
        std::cout << chip.getInfo() << std::endl;
    }

    DDigitalInput input(inPin,chip.handle());
    if (!input.begin(true,0)) {
        std::cerr << "input begin() failed: " << input.getLastError() << std::endl;
        return -1;
    }
    DDigitalOutput output(outPin,chip.handle());
    if (!output.begin(LOW)) {
        std::cerr << "output begin() failed: " << input.getLastError() << std::endl;
        return -2;
    }
    output.write(input.read());

    std::cout << "Start monitoring pin " << inPin << std::endl;
    std::cout << "Pin " << outPin << " is following pin " << inPin << " level" << std::endl;
    std::cout << "Press CTRL+C to stop" << std::endl;
    std::cout << "Current level " << input.read() << std::endl;
    do {
        if (input.isChanged()) {
            std::cout << "Changed to " << input.read() << std::endl;
            output.write(input.read());
        }
        delay(20);
    }while(true);

    return 0;
}
