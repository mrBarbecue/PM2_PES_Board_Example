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
        printf("containerFull\n");
        return true;
    }
    return false;
}

void Container::tiltContainer(bool enable){   //Kippt Behälter, int enable -> Kippt falls true
    if(enable){
        printf("TiltContainer: true\n");
        ServoTilt.setNormalisedPulseWidth(1.0f);
    }
    else{
        printf("TiltContainer: false\n");
        ServoTilt.setNormalisedPulseWidth(0.0f);
    }
}