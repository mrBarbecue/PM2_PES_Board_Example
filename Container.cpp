#include "headers.h"

//Pin an den der Servo angeschlossen ist
#define PinServoTilt PB_D2
//Ultraschallsensor, wann Behälter Vollbeladen ist
#define PinContainerFull PB_D0

Container::Container() :
    ServoTilt(PinServoTilt),
    ContainerFull(PinContainerFull)
{
    ServoTilt.calibratePulseMinMax(servoTiltAngleMin, servoTiltAngleMax);
}

bool Container::containerFull(){
    if(ContainerFull.read() <= triggContainerFull){
        return true;
    }
    else{
        return false;
    }
}

void Container::tiltContainer(bool enable){   //Kippt Behälter, int enable -> Kippt falls true
    if(enable){
        ServoTilt.setNormalisedPulseWidth(1.0f);
    }
    else{
        ServoTilt.setNormalisedPulseWidth(0.0f);
    }
}
