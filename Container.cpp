#include "headers.h"

//Pin an den der Servo angeschlossen ist
#define PinServoTilt PB_D3
//Pin an dem der Ultraschallsensor angeschlossen ist
#define PinContainerFull PB_D0

Container::Container() :
    //Erzeugt Servoobjekt
    ServoTilt(PinServoTilt),
    //Erzeugt Ultraschallobjekt
    ContainerFull(PinContainerFull)
{
    //Setzt min und max Werte für den Servo
    ServoTilt.calibratePulseMinMax(servoTiltAngleMin, servoTiltAngleMax);
    //Aktiviert den Servo
    ServoTilt.enable(0.8f);
}

//Gibt true zurück, falls behälter voll ist
bool Container::containerFull(){
    return false;
    float bufferUltrasonicSensor = ContainerFull.read();
    //printf("UltraSonic: %.2f\n", bufferUltrasonicSensor);
    if(bufferUltrasonicSensor <= triggContainerFull){
        //printf("containerFull\n");
        return true;
    }
    else{
        //printf("containerNotFull\n");
        return false;
    }
}

//Kippt Behälter bei true
void Container::tiltContainer(bool enable){   //Kippt Behälter, int enable -> Kippt falls true
    if(enable){
        //printf("TiltContainer: true\n");
        ServoTilt.setNormalisedPulseWidth(1.0f);
    }
    else{
        //printf("TiltContainer: false\n");
        ServoTilt.setNormalisedPulseWidth(0.0f);
        
    }
}