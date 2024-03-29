#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>
#include <dmpacket>
#include <ddigitalinput>

int main(int argc, char** argv) {

    int Pin=0;
    if (argc == 2) {
        Pin=atoi(argv[1]);
    }
    else {
        std::cout << "Missing Pin argument, must specify Pin nr" << std::endl;
        exit(1);
    }

    if (Pin <= 0 || Pin > 32) {
        std::cout << "Pin nr not valid." << std::endl;
        exit(1);
    }

    DDigitalInput *input=new DDigitalInput(Pin);
    
    std::cout << "Start monitoring pin " << Pin << "..." << std::endl;
    std::cout << "Press CTRL+C to stop" << std::endl;
    do {
        if (input->IsChanged()) {
            std::cout << input->Read() << std::endl;
        }
    }while(true);
    //this_thread::sleep_for(chrono::milliseconds(2000));
    std::cout << "Stop" << std::endl;
    delete input;
    return 0;
}
