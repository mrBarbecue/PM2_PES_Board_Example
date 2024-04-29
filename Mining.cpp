#include "headers.h"

#define PinMotorLiftWheel PB_PWM_M3
#define PinEncoderLiftWheelA PB_ENC_A_M3
#define PinEncoderLiftWheelB PB_ENC_B_M3

//Mechanischer Endschalter (NC), wann sich das Schaufelrad in der unteren Endlage befindet
#define PinWheelLowerPosition PB_D1

//Ausgang um Schaufelrad anzusteuern
#define PinMotorWheel PB_D3

/*
MotorLiftWheel.setVelocity(MotorLiftWheel.getMaxVelocity() * 0.5f); halbe Geschwindigkeit
MotorLiftWheel.setVelocity(1.0f); volle Geschwindigkeit
MotorLiftWheel.setVelocity(standardVelocity); standart Geschwindigkeit
MotorLiftWheel.setRotation(3.0f);
*/

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

    WheelLowerPosition.mode(PullDown);
}

void Mining::spinWheel(bool enable){      //Dreht Schaufelrad, int enable -> Dreht falls true, Stoppt falls false
    MotorWheel.write(enable);
}

bool Mining::initializeMotorLiftWheel(){  //Nullt den Encoder des Motors MotorLiftWheel
    printf("initializeMotorLiftWheel\n");
    printf("%f\n",MotorLiftWheel.getRotation());
    if(!WheelLowerPosition.read()){
        MotorLiftWheel.setVelocity(0.0f);
        wheelLowerPositionRotation = MotorLiftWheel.getRotation();
        return true;
    }
    else if(MotorLiftWheel.getRotation() < (-wheelUpperPosRotation)){ //Watchdog falls Endschalter nicht angiebt
        printf("limitSwitchMissing\n");
    }
    MotorLiftWheel.setVelocity(-maxVelocity); //fährt mit Standartgeschwindigkeit nach unten
    return false;

    
}

bool Mining::lowerWheel(){       //Senkt Schaufelrad, rückgabewert false wenn ganz unten
    if(WheelLowerPosition.read() == false){
        printf("wheelLowerPosition\n");
        printf("%f\n",MotorLiftWheel.getRotation() + wheelLowerPositionRotation);
        MotorLiftWheel.setVelocity(0.0f);
        return false;
    }
    else{
        printf("lowerWheel\n");
        printf("%f\n",MotorLiftWheel.getRotation() + wheelLowerPositionRotation);
        MotorLiftWheel.setVelocity(-standardVelocity);
        return true;
    }
}

bool Mining::WheelToUpperPos(){           //Hebt Schaufelrad in die obere Endlage   (Volle Geschwindigkeit)
    if(equalTo(MotorLiftWheel.getRotation(), wheelUpperPosRotationOff)){
        return true;
    }
    else{
        printf("WheelToUpperPos\n");
        printf("%f\n",MotorLiftWheel.getRotation() + wheelLowerPositionRotation);
        MotorLiftWheel.setRotation(wheelUpperPosRotationOff);
        return false;
    }
}

bool Mining::WheelTo10cm(){                //Senkt oder Hebt Schaufelrad auf 10cm    (Volle Geschwindigkeit)
    if(equalTo(MotorLiftWheel.getRotation(), wheel10cmPosRotationOff)){
        return true;
    }
    else{
        printf("WheelTo10cm\n");
        printf("%f\n",MotorLiftWheel.getRotation() + wheel10cmPosRotation);
        MotorLiftWheel.setRotation(wheel10cmPosRotationOff);
        return false;
    }
}

bool Mining::equalTo(float value1, float value2){
    if((value1 > (value2-value2/100)) && (value1 < (value2+value2/100))){
        return true;
    }
    return false;
}

//Für Tests
float Mining::liftTest(){
    MotorLiftWheel.setVelocity(standardVelocity);
    return MotorLiftWheel.getVoltage();
}