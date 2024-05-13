#include "headers.h"

#define PinMotorLiftWheel PB_PWM_M3
#define PinEncoderLiftWheelA PB_ENC_A_M3
#define PinEncoderLiftWheelB PB_ENC_B_M3

//Mechanischer Endschalter (NC), wann sich das Schaufelrad in der unteren Endlage befindet
#define PinWheelLowerPosition PB_D1

//Ausgang um Schaufelrad anzusteuern
#define PinMotorWheel PB_D2

Mining::Mining() :
    MotorLiftWheel(PinMotorLiftWheel, PinEncoderLiftWheelA, PinEncoderLiftWheelB, 31.25f, 450.0f/12.0f, 12.0f),
    //MotorLiftWheel(PinMotorLiftWheel, PinEncoderLiftWheelA, PinEncoderLiftWheelB, 100.0f, 140.0f/12.0f, 12.0f),
    WheelLowerPosition(PinWheelLowerPosition),  //Endschalter ist NC
    MotorWheel(PinMotorWheel)
{
    // enable the motion planner for smooth movement
    MotorLiftWheel.enableMotionPlanner(true);
    // limitiert Maximumgeschwindigkeit
    MotorLiftWheel.setMaxVelocity(MotorLiftWheel.getMaxPhysicalVelocity() * maxVelocity);

    WheelLowerPosition.mode(PullDown); //Pulldown aktivieren
}

void Mining::spinWheel(bool enable){      //Dreht Schaufelrad, bool enable -> Dreht falls true, Stoppt falls false
    MotorWheel.write(enable);
    printf("MotorWheel: %d\n", MotorWheel.read());
}

bool Mining::initializeMotorLiftWheel(){  //Nullt den Encoder des Motors MotorLiftWheel
    printf("initializeMotorLiftWheel\n");
    printf("LiftWheel rotations: %.4f\n",MotorLiftWheel.getRotation());
    if(!WheelLowerPosition.read()){
        MotorLiftWheel.setVelocity(0.0f);
        wheelLowerPositionRotation = MotorLiftWheel.getRotation();
        wheelUpperPosRotationOff = wheelUpperPosRotation + wheelLowerPositionRotation;
        wheel10cmPosRotationOff = wheel10cmPosRotation + wheelLowerPositionRotation;
        printf("\n\nwheelLowerPositionRotation: %.4f\n\n", wheelLowerPositionRotation);
        
        return true;
    }
    else if(MotorLiftWheel.getRotation() < (-wheelUpperPosRotation)){ //Watchdog falls Endschalter nicht angiebt
        printf("limitSwitchMissing\n");
        return true;
    }
    MotorLiftWheel.setVelocity(-maxVelocityRps); //fährt mit Maxgeschwindigkeit nach unten
    return false;
}

bool Mining::lowerWheel(){       //Senkt Schaufelrad, rückgabewert false wenn ganz unten
    if(!WheelLowerPosition.read()){
        printf("wheelLowerPosition\n");
        MotorLiftWheel.setVelocity(0.0f);
        return false;
    }
    else{
        printf("lowerWheel\n");
        printMotorLiftPos();
        MotorLiftWheel.setVelocity(-standardVelocity);
        return true;
    }
}

bool Mining::WheelToUpperPos(){           //Hebt Schaufelrad in die obere Endlage   (Volle Geschwindigkeit)
    printf("wheelUpperPosRotation: %.4f\n", wheelUpperPosRotationOff);
    printf("MotorLiftWheel rotations: %.4f\n", MotorLiftWheel.getRotation());
    if(equalTo(MotorLiftWheel.getRotation(), wheelUpperPosRotationOff)){
        printf("WheelAtUpperPos\n");
        return true;
    }
    else{
        printf("WheelToUpperPos\n");
        printMotorLiftPos();
        MotorLiftWheel.setRotation(wheelUpperPosRotationOff);
        return false;
    }
}

bool Mining::WheelTo10cm(){                //Senkt oder Hebt Schaufelrad auf 10cm    (Volle Geschwindigkeit)
    printf("wheel10cmPosRotation: %.4f\n", wheel10cmPosRotationOff);
    if(equalTo(MotorLiftWheel.getRotation(), wheel10cmPosRotationOff)){
        printf("WheelAt10cm\n");
        return true;
    }
    else{
        printf("WheelTo10cm\n");
        printMotorLiftPos();
        MotorLiftWheel.setRotation(wheel10cmPosRotationOff);
        return false;
    }
}

bool Mining::equalTo(float value1, float value2){
    if(value1 > value2 - 0.01 && value1 < value2 + 0.01){
        return true;
    }
    return false;
}

void Mining::printMotorLiftPos(){
    printf("LiftWheel position: %.4fmm\n",(MotorLiftWheel.getRotation() - wheelLowerPositionRotation) * ThreadPitch + 10);
}

//Für Tests
float Mining::liftWheel(){
    MotorLiftWheel.setVelocity(7.5f);
    printMotorLiftPos();
    return MotorLiftWheel.getVoltage();
}

bool Mining::getMechanicalSwitch(){
    return !WheelLowerPosition.read();
}