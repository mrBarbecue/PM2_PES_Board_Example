#include "headers.h"

//Pin an den der Servo angeschlossen ist
#define PinServoTilt PB_D3
//Ultraschallsensor, wann Behälter Vollbeladen ist
#define PinContainerFull PB_D0

Container::Container() :
    ServoTilt(PinServoTilt),
    ContainerFull(PinContainerFull)
{
    ServoTilt.calibratePulseMinMax(servoTiltAngleMin, servoTiltAngleMax);
}

bool Container::containerFull(){
    return false;
    /*
    float bufferUltrasonicSensor = ContainerFull.read();
    printf("UltraSonic: %.2f\n", bufferUltrasonicSensor);
    if(bufferUltrasonicSensor <= triggContainerFull){
        printf("containerFull\n");
        return true;
    }
    else{
        printf("containerNotFull\n");
        return false;
    }
    */
}

void Container::tiltContainer(bool enable){   //Kippt Behälter, int enable -> Kippt falls true
    if(enable){
        printf("TiltContainer: true\n");
        //ServoTilt.setNormalisedPulseWidth(1.0f);
        ServoTilt.enable();
    }
    else{
        printf("TiltContainer: false\n");
        ServoTilt.setNormalisedPulseWidth(0.0f);
        ServoTilt.enable(0.8f);
    }
}

float Container::getUltrasonic(){
    printf("Ultrasonic: %.1f\n", ContainerFull.read());
    return ContainerFull.read();
}