//#include <cstdlib>
#include <iostream>
//#include <memory>
//#include <string>
#include <chrono>
#include <thread>
#include "DDCMotor.h"

using namespace std;

int main(int argc, char** argv) {
    DDCMotor *Motor;
    if (argc < 3) {
        cout << "Usage: "+std::string(argv[0])+" <PrmPin nr> <DirPwmPin nr>";
        cout << "[Control mode] -> PHASE_ENABLED or IN_IN (default=PHASE_ENABLE)";
        exit(1);
    }
    
    short PwmPin=atoi(argv[1]);
    short DirPwmPin=atoi(argv[2]);
    DDCMotor::DControlMode Mode=DDCMotor::DControlMode::PHASE_ENABLED;

    if (argc >= 4) {
        if (string(argv[3]) == "IN_IN") {
            Mode=DDCMotor::DControlMode::IN_IN;
        }
    }
        
    
    Motor = new DDCMotor(PwmPin,DirPwmPin,Mode);
    cout << "Speed up fw..." << endl;
    for (int v=1; v<=100 ;v++) {
        Motor->Fw(v);
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    cout << "Max speed reached, wait 2 sec..." << endl;
    this_thread::sleep_for(chrono::milliseconds(2000));
    cout << "Stop" << endl;
    Motor->Stop();
    cout << "Speed up rew..." << endl;
    for (int v=0; v<=100 ;v++) {
        Motor->Rev(v);
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    cout << "Max speed reached, wait 2 sec..." << endl;
    this_thread::sleep_for(chrono::milliseconds(2000));
    cout << "Stop" << endl;
    Motor->Stop();
    delete Motor;

    return 0;
}
