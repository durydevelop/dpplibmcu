#include <iostream>
#include <dpwm>
#include <filesystem>

void showUsage(std::filesystem::path binaryName)
{
    std::cout <<
        "This program output a pwm signal on every pin." << std::endl <<
        "Usage: " << binaryName.stem().string() << " <pin> [frequency] [duty-cycle]" << std::endl <<
        "    <pin>          Output pin." << std::endl <<
        "    [frequency]    Frequency in Hz (default 5 Hz)" << std::endl <<
        "    [duty-cycle]   Duty-cycle in percent (default 50 %)" << std::endl <<
        "    -h, --help     Show this help" << std::endl;
}

int main(int argc, char** argv) {

    int pwmPin=0;
    int freqHz=5;
    int dutyPerc=50;

    if (argc >= 2) {
        std::string sArg(argv[1]);
        if (sArg == "-h" || sArg == "--help") {
            showUsage(argv[0]);
            exit(1);
        }

        pwmPin=atoi(argv[1]);

        if (argc >= 3) {
            freqHz=atoi(argv[2]);
        }

        if (argc >= 4) {
            dutyPerc=atoi(argv[3]);
        }
    }
    else {
        std::cout << "Missing arguments." << std::endl;
        showUsage(argv[0]);
        exit(1);
    }

    if (pwmPin <= 0 || pwmPin > 32) {
        std::cout << "Pin nr not valid." << std::endl;
        exit(1);
    }

    if (freqHz > DPwmOut::MAX_PWM_FREQ) {
        std::cout << "Warning: frequency exceed max value and will be set to " << DPwmOut::MAX_PWM_FREQ << std::endl;
    }

    if (dutyPerc > 100) {
        std::cout << "Warning: duty-cycle value exceed 100 value and will be set to 100" << std::endl;
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
   
    std::cout << "Start " << freqHz << " Hz " << dutyPerc << " % duty-cycle pwm on pin " << pwmPin << std::endl;
    std::cout << "Press CTRL+C to stop" << std::endl;
    
    
    pwm.set(freqHz,dutyPerc,true);

    do{

    }while(true);
    
    return 0;
}
