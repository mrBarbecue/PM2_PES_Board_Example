#include "headers.h"

#define PinServoTilt PB_D2

#define containerFull PB_D0

/*
servoTilt.setNormalisedPulseWidth(servo_input);
*/

class Container{
    private:
        Servo servoTilt;

        float servoTiltAngleMin = 0.0150f;
        float servoTiltAngleMax = 0.1150f;


    
    public:
        Container() : servoTilt(PinServoTilt){
            
            servoTilt.calibratePulseMinMax(servoTiltAngleMin, servoTiltAngleMax);
        }

        void tiltContainer(int enable){   //Kippt Behälter, int enable -> Kippt falls true

        }

    private:


};