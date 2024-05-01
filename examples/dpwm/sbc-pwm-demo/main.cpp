#include <iostream>
//#include <dservo>
//#include <dgpio.h>
#include <dpwm>

int main(int argc, char** argv) {

    int pwmPin=0;
    if (argc == 2) {
        pwmPin=atoi(argv[1]);
    }
    else {
        std::cout << "Missing Pin argument, must specify servo pin nr" << std::endl;
        exit(1);
    }

    if (pwmPin <= 0 || pwmPin > 32) {
        std::cout << "Servo pin nr not valid." << std::endl;
        exit(1);
    }

    /* OK
    int chip = lgGpiochipOpen(0);
    int ret=lgGpioClaimOutput(chip,0,19,0);
    std::cout << "ret=" << ret << std::endl;
    ret=lgTxPulse(chip,  19, 100000, 100000, 0, 0); // flash LED at 5 Hz
    std::cout << "ret=" << ret << std::endl;
    */

   DGpioChip chip(0);
    if (!chip.isReady()) {
        std::cout << "DGpioChip not ready: " << chip.getLastError() << std::endl;
    }
    else {
        std::cout << chip.getInfo() << std::endl;
    }

    DPwmOut pwm(pwmPin,chip.handle());
    pwm.begin();
    std::cout << pwm.getLastError() << std::endl;
   
    std::cout << "Start 5 Hz pwm on pin " << pwmPin << std::endl;
    std::cout << "Press CTRL+C to stop" << std::endl;
    
    
    pwm.set(5,50,true);

    do{

    }while(true);
    
    return 0;
}
