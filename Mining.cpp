#include "headers.h"

//Pins für den MotorLiftWheel
#define PinMotorLiftWheel PB_PWM_M3
#define PinEncoderLiftWheelA PB_ENC_A_M3
#define PinEncoderLiftWheelB PB_ENC_B_M3

//Pin für Mechanischer Endschalter (NC), wann sich das Schaufelrad in der unteren Endlage befindet
#define PinWheelLowerPosition PB_D1

//Ausgang um Schaufelrad anzusteuern
#define PinMotorWheel PB_D2

Mining::Mining() :
    //Erzeugt Motorobjekt
    MotorLiftWheel(PinMotorLiftWheel, PinEncoderLiftWheelA, PinEncoderLiftWheelB, 31.25f, 450.0f/12.0f, 12.0f),
    //Erzeugt einen DigitalIn
    WheelLowerPosition(PinWheelLowerPosition),
    //Erzeugt einen DigitalOut
    MotorWheel(PinMotorWheel)
{
    //Aktiviert motionplanner
    MotorLiftWheel.enableMotionPlanner(true);
    //limitiert Maximumgeschwindigkeit
    MotorLiftWheel.setMaxVelocity(MotorLiftWheel.getMaxPhysicalVelocity() * maxVelocity);
    //Pulldown aktivieren
    WheelLowerPosition.mode(PullDown);
}

//Dreht Schaufelrad, bool enable -> Dreht falls true, Stoppt falls false
void Mining::spinWheel(bool enable){
    MotorWheel.write(enable);
    //printf("MotorWheel: %d\n", MotorWheel.read());
}

//Nullt den Encoder des Motors MotorLiftWheel (Volle Geschwindigketi)
bool Mining::initializeMotorLiftWheel(){
    //printf("initializeMotorLiftWheel\n");
    //printf("LiftWheel rotations: %.4f\n",MotorLiftWheel.getRotation());
    if(!WheelLowerPosition.read()){
        MotorLiftWheel.setVelocity(0.0f);
        wheelLowerPositionRotation = MotorLiftWheel.getRotation();
        wheelUpperPosRotationOff = wheelUpperPosRotation + wheelLowerPositionRotation;
        wheel10cmPosRotationOff = wheel10cmPosRotation + wheelLowerPositionRotation;
        //printf("\n\nwheelLowerPositionRotation: %.4f\n\n", wheelLowerPositionRotation);
        return true;
    }
    else if(MotorLiftWheel.getRotation() < (-wheelUpperPosRotation)){ //Watchdog falls Endschalter nicht angiebt
        //printf("limitSwitchMissing\n");
        return true;
    }
    else{
    MotorLiftWheel.setVelocity(-maxVelocityRps); //fährt mit Maxgeschwindigkeit nach unten
    return false;
    }
}

//Senkt Schaufelrad langsam, rückgabewert true wenn ganz unten
bool Mining::lowerWheel(){       
    if(!WheelLowerPosition.read()){
        //printf("wheelLowerPosition\n");
        MotorLiftWheel.setVelocity(0.0f);
        return true;
    }
    else{
        //printf("lowerWheel\n");
        printMotorLiftPos();
        MotorLiftWheel.setVelocity(-standardVelocity);
        return false;
    }
}

//Hebt Schaufelrad in die obere Endlage (Volle Geschwindigkeit)
bool Mining::wheelToUpperPos(){
    //printf("wheelUpperPosRotation: %.4f\n", wheelUpperPosRotationOff);
    //printf("MotorLiftWheel rotations: %.4f\n", MotorLiftWheel.getRotation());
    if(equalTo(MotorLiftWheel.getRotation(), wheelUpperPosRotationOff)){
        //printf("WheelAtUpperPos\n");
        return true;
    }
    else{
        //printf("WheelToUpperPos\n");
        printMotorLiftPos();
        MotorLiftWheel.setRotation(wheelUpperPosRotationOff);
        return false;
    }
}

//Senkt Schaufelrad auf 10cm (Volle Geschwindigkeit)
bool Mining::wheelTo10cm(){
    //printf("wheel10cmPosRotation: %.4f\n", wheel10cmPosRotationOff);
    if(equalTo(MotorLiftWheel.getRotation(), wheel10cmPosRotationOff)){
        //printf("WheelAt10cm\n");
        return true;
    }
    else{
        //printf("WheelTo10cm\n");
        printMotorLiftPos();
        MotorLiftWheel.setRotation(wheel10cmPosRotationOff);
        return false;
    }
}

//Vergleicht Soll und Zielwerte mit Tolerenz und gibt true zurück falls sie gleich sind
bool Mining::equalTo(float value1, float value2){
    if(value1 > value2 - rotationTolerance && value1 < value2 + rotationTolerance){
        return true;
    }
    return false;
}

void Mining::printMotorLiftPos(){
    //printf("LiftWheel position: %.4fmm\n",(MotorLiftWheel.getRotation() - wheelLowerPositionRotation) * ThreadPitch + 10);
}

//Für Tests
float Mining::liftWheel(){
    MotorLiftWheel.setVelocity(1.0f);
    printMotorLiftPos();
    return MotorLiftWheel.getVoltage();
}

bool Mining::getMechanicalSwitch(){
    return !WheelLowerPosition.read();
}