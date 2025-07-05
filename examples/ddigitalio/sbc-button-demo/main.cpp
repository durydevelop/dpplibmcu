#include <iostream>
#include <ddigitalbutton>
#include <filesystem>

void showUsage(std::filesystem::path binaryName)
{
    std::cout <<
        "This program show a button press status (pressing, pressed, double pressed, long pressed, etc...)." << std::endl <<
        "You only need to connect a button from one GPIO to GND." << std::endl <<
        "Usage: " << binaryName.stem().string() << " <pin>" << std::endl <<
        "    <pin>    GPIO pin connected to the button" << std::endl <<
        "    -h, --help     Show this help" << std::endl;
}

int main(int argc, char** argv) {

    int buttonPin=0;

    if (argc > 1) {
        std::string sArg(argv[1]);
        if (sArg == "-h" || sArg == "--help") {
            showUsage(argv[0]);
            exit(1);
        }
    
        if (argc >= 2) {
            buttonPin=atoi(argv[1]);
        }
    }
    else {
        std::cout << "Missing argument." << std::endl;
        showUsage(argv[0]);
        exit(1);
    }

    if (buttonPin <= 0 || buttonPin > 32) {
        std::cout << "Button pin nr not valid." << std::endl;
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

    DDigitalButton button(buttonPin);
    if (!button.begin()) {
        std::cerr << "button.begin() failed: " << button.getLastError() << std::endl;
        return -2;
    }

    std::cout << "Start monitoring buttonon pin " << buttonPin << std::endl;
    std::cout << "Press CTRL+C to stop" << std::endl;

    do {    
        switch (button.read()) {
            case DDigitalButton::PRESSED:
                std::cout << "<PRESSED>" << std::endl;
                break;
            case DDigitalButton::LONG_PRESSED:
                std::cout << "<LONG_PRESSED>" << std::endl;
                break;
            case DDigitalButton::DBL_PRESSED:
                std::cout << "<DBL_PRESSED>" << std::endl;
                break;
            case DDigitalButton::RELEASED:
                std::cout << "<RELEASED>" << std::endl;
                break;
            case DDigitalButton::DBL_PRESSING:
                std::cout << "<DBL_PRESSING>" << std::endl;
                break;
            case DDigitalButton::PRESS:
                std::cout << "<PRESS>" << std::endl;
                break;
            case DDigitalButton::LONG_PRESSING:
                std::cout << "<LONG_PRESSING>" << std::endl;
                break;
            case DDigitalButton::RELEASE:
                std::cout << "<RELEASE>" << std::endl;
                break;
            default:
                break;
        }
        delay(20);
    }while(true);

    return 0;
}
