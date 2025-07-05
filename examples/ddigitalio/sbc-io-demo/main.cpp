#include <iostream>
#include <ddigitalinput>
#include <ddigitaloutput>
#include <filesystem>

void showUsage(std::filesystem::path binaryName)
{
    std::cout <<
        "This program wait for INPUT pin changes and set OUTPUT pin to same state." << std::endl <<
        "Usage: " << binaryName.stem().string() << " <INPUT pin> <OUTPUT pin>" << std::endl <<
        "    <INPUT pin>    GPIO pin number used for INPUT" << std::endl <<
        "    <OUTPUT pin>   GPIO pin number used for OUTPUT" << std::endl <<
        "    -h, --help     Show this help" << std::endl <<
        "Example:" << std::endl <<
        binaryName.stem().string() << " 4 5" << std::endl <<
        "will out on GPIO5 the GPIO4 level when changes." << std::endl;
}

int main(int argc, char** argv) {

    int inPin=0;
    int outPin=19;

    if (argc > 1) {
        std::string sArg(argv[1]);
        if (sArg == "-h" || sArg == "--help") {
            showUsage(argv[0]);
            exit(1);
        }
    }

    if (argc == 3) {
        inPin=atoi(argv[1]);
        outPin=atoi(argv[2]);
    }
    else {
        std::cout << "Missing arguments." << std::endl;
        showUsage(argv[0]);
        exit(1);
    }

    if (inPin <= 0 || inPin > 32) {
        std::cout << "Input pin nr not valid." << std::endl;
        showUsage(argv[0]);
        exit(1);
    }

    if (outPin <= 0 || outPin > 32) {
        std::cout << "Output pin nr not valid." << std::endl;
        showUsage(argv[0]);
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
