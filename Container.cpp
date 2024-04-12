#include "headers.h"

#define PinServoTilt PB_D2

#define containerFull PB_D0

/*
servoTilt.setNormalisedPulseWidth(servo_input);
*/


Container::Container() : servoTilt(PinServoTilt){
    servoTilt.calibratePulseMinMax(servoTiltAngleMin, servoTiltAngleMax);
}

void Container::tiltContainer(bool enable){   //Kippt Behälter, int enable -> Kippt falls true
}
