#include "headers.h"

#define PinServoTilt PB_D2

//Ultraschallsensor, wann Behälter Vollbealaden ist
#define PinContainerFull PB_D0

/*
servoTilt.setNormalisedPulseWidth(servo_input);
*/


Container::Container() : ServoTilt(PinServoTilt){
    ServoTilt.calibratePulseMinMax(servoTiltAngleMin, servoTiltAngleMax);
}

void Container::tiltContainer(bool enable){   //Kippt Behälter, int enable -> Kippt falls true
}
