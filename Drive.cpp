#include "headers.h"

#define PinMotorDriveRight PB_PWM_M1
#define PinMotorDriveLeft PB_PWM_M2

#define PinEncoderMotorRightA PB_ENC_A_M1
#define PinEncoderMotorRightB PB_ENC_B_M1
#define PinEncoderMotorLeftA PB_ENC_A_M2
#define PinEncoderMotorLeftB PB_ENC_B_M2

#define beforContainer

/*
PWM input 0.0f: -12V
PWM input 0.5f: 0V
PWM input 1.0f: 12V

MotorLiftWheel.setVelocity(MotorLiftWheel.getMaxVelocity() * 0.5f); halbe Geschwindigkeit
MotorLiftWheel.setVelocity(1.0f); volle Geschwindigkeit
MotorLiftWheel.setVelocity(standardVelocity); standart Geschwindigkeit
MotorLiftWheel.setRotation(3.0f);
*/


Drive::Drive() :
    motorDriveRight(PinMotorDriveRight, PinEncoderMotorRightA, PinEncoderMotorRightB, 100.0f, 140.0f/12.0f, 12.0f),
    motorDriveLeft(PinMotorDriveLeft, PinEncoderMotorLeftA, PinEncoderMotorLeftB, 100.0f, 140.0f/12.0f, 12.0f)
{                          
    // enable the motion planner for smooth movement
    motorDriveRight.enableMotionPlanner(true);
    // limitiert Maximumgeschwindigkeit
    motorDriveRight.setMaxVelocity(motorDriveRight.getMaxPhysicalVelocity() * maxVelocity);

    // enable the motion planner for smooth movement
    motorDriveLeft.enableMotionPlanner(true);
    // limitiert Maximumgeschwindigkeit
    motorDriveLeft.setMaxVelocity(motorDriveLeft.getMaxPhysicalVelocity() * maxVelocity);
}

void Drive::nextPosition(int pos){       //Fährt zur nächsten Position vor dem Startbehälter, int pos -> welche position vor dem Behälter

/*
Update the printing command to print the number of rotations:
printf("Motor position: %f \n", motor_M3.getRotation());
Then include the command that will rotate the motor 3 times:
motor_M3.setRotation(3.0f);
*/
}

void Drive::toStartContainer(){          //Fährt zu Startbehälter
}

void Drive::toFinishContainer(){         //Fährt zum Zielbehälter
}