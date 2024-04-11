#include "headers.h"

#define PinMotorLiftWheel PB_PWM_M3
#define PinEncoderLiftWheelA PB_ENC_A_M3
#define PinEncoderLiftWheelB PB_ENC_B_M3

#define PinWheel

#define wheelLowerPosition PB_D1

/*
MotorLiftWheel.setVelocity(MotorLiftWheel.getMaxVelocity() * 0.5f); halbe Geschwindigkeit
MotorLiftWheel.setVelocity(1.0f); volle Geschwindigkeit
MotorLiftWheel.setVelocity(standardVelocity); standart Geschwindigkeit
MotorLiftWheel.setRotation(3.0f);
*/

class Mining{
    private:
        DCMotor MotorLiftWheel;

        const float gearRatioMotorLiftWheel = 31.25f;
        const float rpmV = 450.0f / 12.0f;
        const float voltageMax = 12.0f;
        const float maxVelocity = 1.0f;
        const float standardVelocity = 8.13f / voltageMax; // soll 350rpm, 5rps
        const float threadedRodlengtMm = ???f;
        const float wheelUpperPos = threadedRodlengtMm / (1.25f * gearRatioMotorLiftWheel * 20); //Encoder Position Rad ganz oben, (20 weil 20 Encoderimpulse/umdrehung)

    
    public:
        Mining() : MotorLiftWheel(PinMotorLiftWheel, PinEncoderLiftWheelA, PinEncoderLiftWheelB, gearRatioMotorLiftWheel, rpmV, voltageMax){

            // enable the motion planner for smooth movement
            MotorLiftWheel.enableMotionPlanner(true);
            // limitiert Maximumgeschwindigkeit
            MotorLiftWheel.setMaxVelocity(MotorLiftWheel.getMaxPhysicalVelocity() * maxVelocity);
        }
        void spinnWheel(int enable){      //Dreht Schaufelrad, int enable -> Dreht falls true, Stoppt falls false

        }
        void initializeMotorLiftWheel(){  //Nullt den Encoder des Motors MotorLiftWheel
            MotorLiftWheel.setVelocity(-1.0f); //fährt mit voller Geschwindigkeit nach unten
            while(wheelLowerPosition != 0);
            MotorLiftWheel.setVelocity(0.0f);
            MotorLiftWheel.setEncoderToZero();
        }
        int lowerWheel(int enable){       //Senkt Schaufelrad, int enable -> Senkt falls true, Stoppt bei false, rückgabewert -> false, wenn untere pos erreicht

        return 9999999999999999999;
        }
        void WheelToUpperPos(){           //Hebt Schaufelrad in die obere Endlage   (Volle Geschwindigkeit)

        }
        void WheelTo10cm(){                //Senkt oder Hebt Schaufelrad auf 10cm    (Volle Geschwindigkeit)

        }

    private:


};