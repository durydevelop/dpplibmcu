#include <iostream>
#include <chrono>
#include <thread>
#include "DDigitalInput.h"

using namespace std;

int main(int argc, char** argv) {

    int Pin=0;
    if (argc == 2) {
        Pin=atoi(argv[1]);
    }
    else {
        cout << "Missing Pin argument, must specify Pin nr" << endl;
        exit(1);
    }

    if (Pin <= 0 || Pin > 32) {
        cout << "Pin nr not valid." << endl;
        exit(1);
    }

    DDigitalInput *input=new DDigitalInput(Pin);
    
    cout << "Start monitoring pin " << Pin << "..." << endl;
    cout << "Press CTRL+C to stop" << endl;
    do {
        if (input->IsChanged()) {
            cout << input->Read() << endl;
        }
    }while(true);
    //this_thread::sleep_for(chrono::milliseconds(2000));
    cout << "Stop" << endl;
    delete input;
    return 0;
}
