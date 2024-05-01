#include "WheelsDiag.h"

#define map(x,in_min,in_max,out_min,out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min

void WheelsDiag(DDCMotorWheels *dcWheels, DNCurses *nc)
{
    DNCurses::DWindow *wDiag=nc->CreateWindow(2,2,16,68);
    wDiag->AddBox();
    wDiag->Update();

    wDiag->Printf(2,2,"\t\t\tWheels Diagnostic");
    wDiag->Printf(4,2,"SX M1 Pins: %d  %d",dcWheels->GetM1aPin(),dcWheels->GetM1bPin());
    wDiag->Printf(6,2,"q -> SX Motor Speed UP");
    wDiag->Printf(7,2,"a -> SX Motor Speed STOP");
    wDiag->Printf(8,2,"z -> SX Motor Speed DOWN");
    wDiag->Printf(4,40,"DX M2 Pins: %d  %d",dcWheels->GetM2aPin(),dcWheels->GetM2bPin());
    wDiag->Printf(6,40,"w -> DX Motor Speed UP");
    wDiag->Printf(7,40,"s -> DX Motor Speed STOP");
    wDiag->Printf(8,40,"x -> DX Motor Speed DOWN");
    wDiag->Printf(15,20,"p -> Calibrate Wheels Balange");

    wchar_t ch;
    do {
        // M1 Status
        int M1Set=dcWheels->M1GetCurrSet();
        wDiag->Printf(12,10,"%d%%  ",abs(M1Set));
        if (M1Set > 0) {
            wDiag->Printf(11,11,"^");
            wDiag->Printf(13,11," ");
        }
        else if (M1Set < 0) {
            wDiag->Printf(11,11," ");
            wDiag->Printf(13,11,"v");
        }
        else {
            wDiag->Printf(11,11," ");
            wDiag->Printf(13,11," ");
        }

        // M2 Status
        int M2Set=dcWheels->M2GetCurrSet();
        wDiag->Printf(12,48,"%d%%  ",abs(M2Set));
        if (M2Set > 0) {
            wDiag->Printf(11,49,"^");
            wDiag->Printf(13,49," ");
        }
        else if (M2Set < 0) {
            wDiag->Printf(11,49," ");
            wDiag->Printf(13,49,"v");
        }
        else {
            wDiag->Printf(11,49," ");
            wDiag->Printf(13,49," ");
        }

        wDiag->Update();
        ch=wDiag->Getch();
        switch (ch) {
            case 'q':
                dcWheels->M1FwStep();
                break;
            case 'a':
                dcWheels->M1Brake();
                break;
            case 'z':
                dcWheels->M1RevStep();
                break;
            case 'w':
                dcWheels->M2FwStep();
                break;
            case 's':
                dcWheels->M2Brake();
                break;
            case 'x':
                dcWheels->M2RevStep();
                break;
            case 'p':
                WheelsCalibrate(dcWheels,nc);
            default:
                dcWheels->ALLStop();
                break;
        }
    }while(ch != KEY_BACKSPACE);

    delete wDiag;
}

#define GetMapBalance(x) map(x,-255,255,2,75);
void WheelsCalibrate(DDCMotorWheels *dcWheels, DNCurses *nc)
{
    DNCurses::DWindow *wDiag=nc->CreateWindow(2,2,22,78);
    wDiag->AddBox();
    wDiag->Update();

    wDiag->Printf(2,2,"\t\t\t\tWheels Balancing");
    wDiag->Printf(4,2,"w -> Increment speed");
    wDiag->Printf(5,2,"s -> Stop wheels");
    wDiag->Printf(6,2,"x -> Decrement speed");
    wDiag->Printf(7,2,"a -> Balange Vel to Left");
    wDiag->Printf(8,2,"d -> Balange Vel to Right");
    wDiag->Printf(7,2,"z -> Balange PWM to Left");
    wDiag->Printf(8,2,"c -> Balange PWM to Right");
    wDiag->Printf(5,2,"v -> Reset Balange");
    wDiag->Printf(19,1,"<-------------------------------------------------------------------------->");

    // Intial values
    short int M1Set=dcWheels->M1GetCurrSet();
    short int M2Set=dcWheels->M2GetCurrSet();
    short int M1Pwm=dcWheels->M1GetCurrPwm();
    short int M2Pwm=dcWheels->M2GetCurrPwm();
    short int MappedBal=GetMapBalance(0);
    wchar_t ch;
    do {
        // M1 Status
        M1Set=dcWheels->M1GetCurrSet();
        M1Pwm=dcWheels->M1GetCurrPwm();
        wDiag->Printf(13,5,"Max: Vel %3d Pwm %3d    ",dcWheels->M1GetCalibrationVelFw(),dcWheels->M1GetCalibrationPwmFw());
        wDiag->Printf(14,5,"Set: Vel %3d Pwm %3d    ",M1Set,M1Pwm);
        wDiag->Printf(15,5,"Max: Vel %3d Pwm %3d    ",dcWheels->M1GetCalibrationVelRev(),dcWheels->M1GetCalibrationPwmRev());
        if (M1Set > 0) {
            wDiag->Printf(13,2,"^");
            wDiag->Printf(14,2,"|");
            wDiag->Printf(15,2," ");
        }
        else if (M1Set < 0) {
            wDiag->Printf(13,2," ");
            wDiag->Printf(14,2,"|");
            wDiag->Printf(15,2,"v");
        }
        else {
            wDiag->Printf(13,2," ");
            wDiag->Printf(14,2,"|");
            wDiag->Printf(15,2," ");
        }

        // M2 Status
        M2Set=dcWheels->M2GetCurrSet();
        M2Pwm=dcWheels->M2GetCurrPwm();
        wDiag->Printf(13,44,"Max: Vel %3d Pwm %3d",dcWheels->M2GetCalibrationVelFw(),dcWheels->M2GetCalibrationPwmFw());
        wDiag->Printf(14,44,"Set: Vel %3d Pwm %3d",M2Set,M2Pwm);
        wDiag->Printf(15,44,"Max  Vel %3d Pwm %3d",dcWheels->M2GetCalibrationVelRev(),dcWheels->M2GetCalibrationPwmRev());
        if (M2Set > 0) {
            wDiag->Printf(13,41,"^");
            wDiag->Printf(14,41,"|");
            wDiag->Printf(15,41," ");
        }
        else if (M2Set < 0) {
            wDiag->Printf(13,41," ");
            wDiag->Printf(14,41,"|");
            wDiag->Printf(15,41,"v");
        }
        else {
            wDiag->Printf(13,41," ");
            wDiag->Printf(14,41,"|");
            wDiag->Printf(15,41," ");
        }

        // Balance
        wDiag->Printf(19,MappedBal,"-");
        unsigned short int M1MaxPwm=M1Set>0 ? dcWheels->M1GetCalibrationPwmFw() : dcWheels->M1GetCalibrationPwmRev();
        unsigned short int M2MaxPwm=M2Set>0 ? dcWheels->M2GetCalibrationPwmFw() : dcWheels->M2GetCalibrationPwmRev();
        MappedBal=GetMapBalance(M1MaxPwm-M2MaxPwm);
        wDiag->Printf(20,36,"%3d ",M1Pwm-M2Pwm);
        wDiag->Printf(19,MappedBal,"^");

        wDiag->Update();
        ch=wDiag->Getch();
        switch (ch) {
            case 'w':
                dcWheels->ALLFwStep();
                break;
            case 's':
                dcWheels->ALLBrake();
                break;
            case 'x':
                dcWheels->ALLRevStep();
                break;
            case 'a':
                // Vel Balancing to M1 (SX Balancing)
                if (M1Pwm <= M2Pwm) {
                    // Same calibration or Already M1 decreased:
                    // Decrease M1
                    if (M1Set > 0) {
                        // M1 is running fw
                        dcWheels->M1SetCalibrationVelFw(dcWheels->M1GetCalibrationVelFw()-2);
                    }
                    else if (M1Set < 0) {
                            // M1 is running rev
                        dcWheels->M1SetCalibrationVelRev(dcWheels->M1GetCalibrationVelRev()-2);
                    }
                }
                else if (M2Pwm < M1Pwm) {
                    // Already M2 decreased:
                    // Increase M2
                    if (M2Set > 0) {
                        // M2 is running fw
                        dcWheels->M2SetCalibrationVelFw(dcWheels->M2GetCalibrationVelFw()+2);
                    }
                    else if (M1Set < 0) {
                        // M2 is running rev
                        dcWheels->M2SetCalibrationVelRev(dcWheels->M2GetCalibrationVelRev()+2);
                    }
                }
                break;
            case 'd':
                // Vel balancing to M2 (DX Balancing)
                if (M1Pwm < M2Pwm) {
                    // Already M1 decresed:
                    // Increase M1
                    if (M1Set > 0) {
                        // M1 is running fw
                        dcWheels->M1SetCalibrationVelFw(dcWheels->M1GetCalibrationVelFw()+2);
                    }
                    else if (M1Set < 0) {
                        // M1 is running rev
                        dcWheels->M1SetCalibrationVelRev(dcWheels->M1GetCalibrationVelRev()+2);
                    }
                }
                else if (M2Pwm <= M1Pwm) {
                    // Same vel or Already M2 decresed:
                    // Decrease M2
                    if (M2Set >= 0) {
                        // M2 is running fw
                        dcWheels->M2SetCalibrationVelFw(dcWheels->M2GetCalibrationVelFw()-2);
                    }
                    else if (M2Set < 0) {
                        // M2 is running rev
                        dcWheels->M2SetCalibrationVelRev(dcWheels->M1GetCalibrationVelRev()-2);
                    }
                }
                break;
            case 'z':
            	// PWM balancing to M1 (SX Balancing)
                if (M1Pwm <= M2Pwm) {
                    // Same calibration or Already M1 decreased:
                    // Decrease M1
                    if (M1Set > 0) {
                        // M1 is running fw
                        dcWheels->M1SetCalibrationPwmFw(dcWheels->M1GetCalibrationPwmFw()-2);
                    }
                    else if (M1Set < 0) {
                            // M1 is running rev
                        dcWheels->M1SetCalibrationPwmRev(dcWheels->M1GetCalibrationPwmRev()-2);
                    }
                }
                else if (M2Pwm < M1Pwm) {
                    // Already M2 decreased:
                    // Increase M2
                    if (M2Set > 0) {
                        // M2 is running fw
                        dcWheels->M2SetCalibrationPwmFw(dcWheels->M2GetCalibrationPwmFw()+2);
                    }
                    else if (M1Set < 0) {
                        // M2 is running rev
                        dcWheels->M2SetCalibrationPwmRev(dcWheels->M2GetCalibrationPwmRev()+2);
                    }
                }
                break;
                break;
            case 'c':
                // PWM balancing to M2 (DX Balancing)
                if (M1Pwm < M2Pwm) {
                    // Already M1 decresed:
                    // Increase M1
                    if (M1Set > 0) {
                        // M1 is running fw
                        dcWheels->M1SetCalibrationPwmFw(dcWheels->M1GetCalibrationPwmFw()+2);
                    }
                    else if (M1Set < 0) {
                        // M1 is running rev
                        dcWheels->M1SetCalibrationPwmRev(dcWheels->M1GetCalibrationPwmRev()+2);
                    }
                }
                else if (M2Pwm <= M1Pwm) {
                    // Same vel or Already M2 decresed:
                    // Decrease M2
                    if (M2Set >= 0) {
                        // M2 is running fw
                        dcWheels->M2SetCalibrationPwmFw(dcWheels->M2GetCalibrationPwmFw()-2);
                    }
                    else if (M2Set < 0) {
                        // M2 is running rev
                        dcWheels->M2SetCalibrationPwmRev(dcWheels->M1GetCalibrationPwmRev()-2);
                    }
                }
                break;
            default:
                dcWheels->ALLStop();
                break;
        }
    }while(ch != KEY_BACKSPACE);

    delete wDiag;
}
