#include <iostream>
#include <cstdlib>
#include <ddcmotorwheels>
#include "DNCurses.h"

void WheelsDiag(DDCMotorWheels *dcWheels, DNCurses *nc);
void WheelsCalibrate(DDCMotorWheels *dcWeels, DNCurses *nc);

#define map(x,in_min,in_max,out_min,out_max) (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min
#define GetMapBalance(x) map(x,-255,255,2,75)

void WheelsDiag(DDCMotorWheels *dcWheels, DNCurses *nc)
{
    DNCurses::DWindow *wDiag=nc->CreateWindow(2,2,16,68);
    wDiag->AddBox();
    wDiag->Update();

    wDiag->Printf(2,2,"\t\t\tWheels Diagnostic");
    wDiag->Printf(4,2,"SX M1 Pins: %d  %d",dcWheels->MotorSX->GetPinPwm(),dcWheels->MotorSX->GetPinDirPwm());
    wDiag->Printf(6,2,"q -> SX Motor Speed UP");
    wDiag->Printf(7,2,"a -> SX Motor Speed STOP");
    wDiag->Printf(8,2,"z -> SX Motor Speed DOWN");
    wDiag->Printf(4,40,"DX M2 Pins: %d  %d",dcWheels->MotorDX->GetPinPwm(),dcWheels->MotorDX->GetPinDirPwm());
    wDiag->Printf(6,40,"w -> DX Motor Speed UP");
    wDiag->Printf(7,40,"s -> DX Motor Speed STOP");
    wDiag->Printf(8,40,"x -> DX Motor Speed DOWN");
    wDiag->Printf(15,20,"p -> Calibrate Wheels Balange");

    wchar_t ch;
    do {
        // SX Status
        int M1Set=dcWheels->SXGetVel();
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

        // DX Status
        int M2Set=dcWheels->DXGetVel();
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
                dcWheels->SXIncVel();
                break;
            case 'a':
                dcWheels->SXBrake();
                break;
            case 'z':
                dcWheels->SXDecVel();
                break;
            case 'w':
                dcWheels->DXIncVel();
                break;
            case 's':
                dcWheels->DXBrake();
                break;
            case 'x':
                dcWheels->DXDecVel();
                break;
            case 'p':
                WheelsCalibrate(dcWheels,nc);
            default:
                dcWheels->Stop();
                break;
        }
    }while(ch != KEY_BACKSPACE);

    delete wDiag;
}

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
    short int SXSet=dcWheels->SXGetVel();
    short int DXSet=dcWheels->DXGetVel();
    short int SXPwm=dcWheels->SXGetPwm();
    short int DXPwm=dcWheels->DXGetPwm();
    short int MappedBal=GetMapBalance(0);
    wchar_t ch;
    do {
        // Motor SX Status
        SXSet=dcWheels->SXGetVel();
        SXPwm=dcWheels->SXGetPwm();
        wDiag->Printf(13,5,"Max: Vel +100 Pwm %3d    ",dcWheels->MotorSX->GetPwmLimitFw());
        wDiag->Printf(14,5,"Set: Vel  %3d Pwm %3d    ",SXSet,SXPwm);
        wDiag->Printf(15,5,"Max: Vel -100 Pwm %3d    ",dcWheels->MotorSX->GetPwmLimitRev());
        if (SXSet > 0) {
            wDiag->Printf(13,2,"^");
            wDiag->Printf(14,2,"|");
            wDiag->Printf(15,2," ");
        }
        else if (SXSet < 0) {
            wDiag->Printf(13,2," ");
            wDiag->Printf(14,2,"|");
            wDiag->Printf(15,2,"v");
        }
        else {
            wDiag->Printf(13,2," ");
            wDiag->Printf(14,2,"|");
            wDiag->Printf(15,2," ");
        }

        // Motor DX Status
        int M2Set=dcWheels->DXGetVel();
        int M2Pwm=dcWheels->DXGetPwm();
        wDiag->Printf(13,44,"Max: Vel +100 Pwm %3d",dcWheels->MotorDX->GetPwmLimitFw());
        wDiag->Printf(14,44,"Set: Vel  %3d Pwm %3d",M2Set,M2Pwm);
        wDiag->Printf(15,44,"Max  Vel -100 Pwm %3d",dcWheels->MotorDX->GetPwmLimitRev());
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
        unsigned short int SXMaxPwm=SXSet>0 ? dcWheels->MotorSX->GetPwmLimitFw() : dcWheels->MotorSX->GetPwmLimitRev();
        unsigned short int DXMaxPwm=DXSet>0 ? dcWheels->MotorDX->GetPwmLimitFw() : dcWheels->MotorDX->GetPwmLimitRev();
        MappedBal=GetMapBalance(SXMaxPwm-DXMaxPwm);
        wDiag->Printf(20,36,"%3d ",SXPwm-DXPwm);
        wDiag->Printf(19,MappedBal,"^");

        wDiag->Update();
        ch=wDiag->Getch();
        switch (ch) {
            case 'w':
                dcWheels->Fw();
                break;
            case 's':
                dcWheels->Brake();
                break;
            case 'x':
                dcWheels->Rev();
                break;
            case 'a':
                // Vel Balancing to Motor SX
                if (SXPwm <= DXPwm) {
                    // Same calibration or Already Motor SX decreased:
                    // Decrease Motor SX
                    if (SXSet > 0) {
                        // Motor SX is running fw
                        dcWheels->MotorSX->DecPwmLimitFw();
                    }
                    else if (SXSet < 0) {
                        // Motor SX is running rev
                        dcWheels->MotorSX->DecPwmLimitRev();
                    }
                }
                else if (DXPwm < SXPwm) {
                    // Already Motor DX decreased:
                    // Increase Motor DX
                    if (DXSet > 0) {
                        // Motor DX is running fw
                        dcWheels->MotorDX->IncPwmLimitFw();
                    }
                    else if (DXSet < 0) {
                        // Motor DX is running rev
                        dcWheels->MotorDX->IncPwmLimitRev();
                    }
                }
                break;
            case 'd':
                // Vel balancing to M2 (DX Balancing)
                if (SXPwm < DXPwm) {
                    // Already Motor SX decresed:
                    // Increase Motor SX
                    if (SXSet > 0) {
                        // Motor SX is running fw
                        dcWheels->MotorSX->IncPwmLimitFw();
                    }
                    else if (SXSet < 0) {
                        // MOtor SX is running rev
                        dcWheels->MotorSX->IncPwmLimitRev();
                    }
                }
                else if (DXPwm <= SXPwm) {
                    // Same vel or Already Motor DX decresed:
                    // Decrease Motor DX
                    if (DXSet >= 0) {
                        // Motor DX is running fw
                        dcWheels->MotorDX->DecPwmLimitFw();
                    }
                    else if (DXSet < 0) {
                        // Motor DX is running rev
                        dcWheels->MotorDX->DecPwmLimitRev();
                    }
                }
                break;
            case 'z':
            	// PWM balancing to M1 (SX Balancing)
                if (SXPwm <= DXPwm) {
                    // Same calibration or Already M1 decreased:
                    // Decrease Motor SX
                    if (SXSet > 0) {
                        // Motor SX is running fw
                        dcWheels->MotorSX->DecPwmLimitFw();
                    }
                    else if (SXSet < 0) {
                        // Motor SX is running rev
                        dcWheels->MotorSX->DecPwmLimitRev();
                    }
                }
                else if (DXPwm < SXPwm) {
                    // Already Motor DX decreased:
                    // Increase Motor DX
                    if (DXSet > 0) {
                        // Motor DX is running fw
                        dcWheels->MotorDX->IncPwmLimitFw();
                    }
                    else if (DXSet < 0) {
                        // Motor DX is running rev
                        dcWheels->MotorDX->IncPwmLimitRev();
                    }
                }
                break;
            case 'c':
                // PWM balancing to M2 (DX Balancing)
                if (SXPwm < DXPwm) {
                    // Already Motor SX decreased:
                    // Increase Motor DX
                    if (DXSet > 0) {
                        // Motor DX is running fw
                        dcWheels->MotorDX->IncPwmLimitFw();
                    }
                    else if (SXSet < 0) {
                        // Motor SX is running rev
                        dcWheels->MotorSX->IncPwmLimitRev();
                    }
                }
                else if (DXPwm <= SXPwm) {
                    // Same vel or Already Motor DX decreased:
                    // Decrease Motor DX
                    if (DXSet >= 0) {
                        // Motor DX is running fw
                        dcWheels->MotorDX->DecPwmLimitFw();
                    }
                    else if (DXSet < 0) {
                        // Motor DX is running rev
                        dcWheels->MotorDX->DecPwmLimitRev();
                    }
                }
                break;
            default:
                dcWheels->Stop();
                break;
        }
    }while(ch != KEY_BACKSPACE);

    delete wDiag;
}

int main(int argc, char** argv) {
    DDCMotorWheels dcWheels(2,3,4,5);
    DNCurses nc;
    WheelsDiag(&dcWheels,&nc);
}
