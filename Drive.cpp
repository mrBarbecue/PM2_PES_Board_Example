#include "headers.h"

#define PinMotorDriveRight PB_PWM_M1
#define PinMotorDriveLeft PB_PWM_M2

#define PinEncoderMotorRightA PB_ENC_A_M1
#define PinEncoderMotorRightB PB_ENC_B_M1
#define PinEncoderMotorLeftA PB_ENC_A_M2
#define PinEncoderMotorLeftB PB_ENC_B_M2

#define PinBeforContainer

#ifndef PI
#define PI 3.1415926
#endif

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
    motorDriveLeft(PinMotorDriveLeft, PinEncoderMotorLeftA, PinEncoderMotorLeftB, 100.0f, 140.0f/12.0f, 12.0f),
    beforContainer(PinBeforContainer)
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

void Drive::initializeDriveMotors(){    //Setzt Koordinaten
    printf("initializeDriveMotors\n");
    motorDriveLeft.setVelocity(1.0f);
    motorDriveRight.setVelocity(1.0f);
    while(beforContainer.read() < triggBeforeContainer); //Fährt solange geradeaus, bis vor Container
    motorDriveLeft.setVelocity(0.0f);
    motorDriveRight.setVelocity(0.0f);
}

void Drive::changeAngleRel(float angle){   //Berechnung in Grad
    printf("changeAngleRel\n");
    //Berechnung anzahl Umdrehung um Roboter um entsprechenden Winkel zu drehen
    rotationsRobot = (angle / 360.0f * axialDistance) / wheelDiameter;
    
    MotorDriveLeft.setRotation(MotorDriveLeft.getRotation() + rotationsRobot);
    MotorDriveLeft.setRotation(MotorDriveRight.getRotation() + rotationsRobot);

    lastAngle = angle; //setzt neuen Winkel
    currentAngle += lastAngle;

    if(currentAngle > 360.0f){
        currentAngle -= 360.0f;
    }
    if(currentAngle < 0){
        currentAngle += 360;
    }
}

void Drive::changeAngleAbs(float angle){   //Berechnung in Rad
    printf("changeAngleAbs\n");
    //Berechnung anzahl Umdrehung um Roboter um entsprechenden Winkel zu drehen
    rotationsRobot = ((angle - currentAngle * PI / 180.0f) / 2 * PI * axialDistance) / wheelDiameter;
    
    MotorDriveLeft.setRotation(MotorDriveLeft.getRotation() + rotationsRobot);
    MotorDriveLeft.setRotation(MotorDriveRight.getRotation() + rotationsRobot);
}

void Drive::driveStraight(int distance){
    printf("driveStraight\n");
    //Speichert anzahl umdrehungen für Distanz
    rotationsDistance = distance / (wheelDiameter * PI)
    MotorDriveLeft.setRotation(MotorDriveLeft.getRotation() + rotationsDistance);
    MotorDriveLeft.setRotation(MotorDriveRight.getRotation() + rotationsDistance);
    //calculatePos(int distance);
}

void Drive::calculatePos(int distance){ //Berechnung in rad
    // +---> X
    // |
    // v
    // Y

    currentPosX += cos(lastAngle * 180.0f / PI) * distance;
    currentPosY -= sin(lastAngle * 180.0f / PI) * distance;
    printf("currentPosX: %f\n", currentPosX);
    printf("currentPosY: %f\n", currentPosY);

    /*
    //1. Quadrant (+x, -y)
    if(currentAngle >= 0 && currentAngle < 90){
        currentPosX += cos(lastAngle * 180.0f / PI) * distance;
        currentPosY -= sin(lastAngle * 180.0f / PI) * distance;
    }
    //2. Quadrant (-x, -y)
    else if(currentAngle >= 90 && currentAngle < 180){
        currentPosX -= sin((lastAngle-90) * 180.0f / PI) * distance;
        currentPosY -= cos((lastAngle-90) * 180.0f / PI) * distance;
    }
    //3. Quadrant (-x, +y)
    else if(currentAngle >= 180 && currentAngle < 270){
        currentPosX -= cos((lastAngle-180) * 180.0f / PI) * distance;
        currentPosY += sin((lastAngle-180) * 180.0f / PI) * distance;
    }
    //4. Quadrant (+x, +y)
    else if(currentAngle >= 270 && currentAngle < 360){
        currentPosX += sin((lastAngle-270) * 180.0f / PI) * distance;
        currentPosY += cos((lastAngle-270) * 180.0f / PI) * distance;
    }
    else{
        printf("currentAngle oder calculatePos falsch berechnet\n");
    }
    */
}

void Drive::driveTo(int x, int y){
    changeAngleAbs(atan((y - currentPosY) / (x - currentPosX)));
    if(MotorDriveLeft.getVelocity == 0.0f){
    driveStraight(sqrt(pow(x - currentPosX, 2) + pow(y - currentPosY, 2)));
    }
}

int Drive::getPosX(){
    return currentPosX;
}

int Drive::getPosY(){
    return currentPosY;
}

void Drive::nextPosition(int pos){       //Fährt zur nächsten Position vor dem Startbehälter, int pos -> welche position vor dem Behälter
}

void Drive::toStartContainer(){          //Fährt zu Startbehälter
}

void Drive::toFinishContainer(){         //Fährt zum Zielbehälter
}