#include "headers.h"

#define PinMotorLiftWheel PB_PWM_M3
#define PinEncoderLiftWheelA PB_ENC_A_M3
#define PinEncoderLiftWheelB PB_ENC_B_M3

#define PinWheel

/*
MotorLiftWheel.setVelocity(MotorLiftWheel.getMaxVelocity() * 0.5f); halbe Geschwindigkeit
MotorLiftWheel.setVelocity(1.0f); volle Geschwindigkeit
MotorLiftWheel.setVelocity(standardVelocity); standart Geschwindigkeit
MotorLiftWheel.setRotation(3.0f);
*/

Mining::Mining() :
    //MotorLiftWheel(PinMotorLiftWheel, PinEncoderLiftWheelA, PinEncoderLiftWheelB, 31.25f, 450.0f/12.0f, 12.0f),
    MotorLiftWheel(PinMotorLiftWheel, PinEncoderLiftWheelA, PinEncoderLiftWheelB, 100.0f, 140.0f/12.0f, 12.0f),
    wheelLowerPosition(PB_D1)  //Endschalter ist NC
{
    // enable the motion planner for smooth movement
    MotorLiftWheel.enableMotionPlanner(true);
    // limitiert Maximumgeschwindigkeit
    MotorLiftWheel.setMaxVelocity(MotorLiftWheel.getMaxPhysicalVelocity() * maxVelocity);

    wheelLowerPosition.mode(PullDown);
}

void Mining::spinnWheel(bool enable){      //Dreht Schaufelrad, int enable -> Dreht falls true, Stoppt falls false
    //PinWheel = enable;
}

void Mining::initializeMotorLiftWheel(){  //Nullt den Encoder des Motors MotorLiftWheel
    printf("initializeMotorLiftWheel\n");
    MotorLiftWheel.setVelocity(-1.0f); //fährt mit voller Geschwindigkeit nach unten
    while(wheelLowerPosition.read() == true){
        printf("%f\n",MotorLiftWheel.getRotation());
        if(MotorLiftWheel.getRotation() < (-wheelUpperPosRotation)){ //Watchdog falls Endschalter nicht angiebt
            printf("limitSwitchMissing\n");
            break;
        }
    }
    MotorLiftWheel.setVelocity(0.0f);
    wheelLowerPositionRotation = MotorLiftWheel.getRotation();
}

bool Mining::lowerWheel(){       //Senkt Schaufelrad, rückgabewert false wenn ganz unten
    if(wheelLowerPosition.read() == false){
        printf("wheelLowerPosition\n");
        printf("%f\n",MotorLiftWheel.getRotation());
        MotorLiftWheel.setVelocity(0.0f);
        return 0;
    }
    else{
        printf("lowerWheel\n");
        printf("%f\n",MotorLiftWheel.getRotation());
        MotorLiftWheel.setVelocity(-standardVelocity);
        return 1;
    }
}

void Mining::WheelToUpperPos(){           //Hebt Schaufelrad in die obere Endlage   (Volle Geschwindigkeit)
    printf("WheelToUpperPos\n");
    printf("%f\n",MotorLiftWheel.getRotation());
    MotorLiftWheel.setRotation(wheelUpperPosRotationOff);
}

void Mining::WheelTo10cm(){                //Senkt oder Hebt Schaufelrad auf 10cm    (Volle Geschwindigkeit)
    printf("WheelTo10cm\n");
    printf("%f\n",MotorLiftWheel.getRotation());
    MotorLiftWheel.setRotation(wheel10cmPosRotationOff);
}