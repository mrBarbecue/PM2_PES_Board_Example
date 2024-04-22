#include "headers.h"

#define PinMotorDriveRight PB_PWM_M1
#define PinMotorDriveLeft PB_PWM_M2

#define PinEncoderMotorRightA PB_ENC_A_M1
#define PinEncoderMotorRightB PB_ENC_B_M1
#define PinEncoderMotorLeftA PB_ENC_A_M2
#define PinEncoderMotorLeftB PB_ENC_B_M2

#define PinInFrontOfContainer

#define PI 3.1415926f

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
    MotorDriveRight(PinMotorDriveRight, PinEncoderMotorRightA, PinEncoderMotorRightB, 100.0f, 140.0f/12.0f, 12.0f),
    MotorDriveLeft(PinMotorDriveLeft, PinEncoderMotorLeftA, PinEncoderMotorLeftB, 100.0f, 140.0f/12.0f, 12.0f),
    InFrontOfContainer(PinInFrontOfContainer)
{                          
    // enable the motion planner for smooth movement
    MotorDriveLeft.enableMotionPlanner(true);
    // limitiert Maximumgeschwindigkeit
    MotorDriveLeft.setMaxVelocity(MotorDriveLeft.getMaxPhysicalVelocity() * maxVelocity);

    // enable the motion planner for smooth movement
    MotorDriveRight.enableMotionPlanner(true);
    // limitiert Maximumgeschwindigkeit
    MotorDriveRight.setMaxVelocity(MotorDriveRight.getMaxPhysicalVelocity() * maxVelocity);
}

void Drive::calculatePositions(){
    //Speichert die Distanz zwischen den einzelnen X-Positionen falls amountOfPositions != 1
    int distanceX = 0;
    switch(amountOfPositions){
        case 1:
            //Weist Position Startwert zu
            positionsX[0] = startPosX;
            positionsY[0] = startAreaYOffset;
            break;
        case 3:
        case 5:
        case 7:
        //Berechnet die Distanz zwischen den einzelnen X-Positionen
        distanceX = startAreaX / (amountOfPositions-1);
        for(int i = 0; i < amountOfPositions; i++){
            //Weist X-Positionen Koordinate mit gleichem Abstand zu
            positionsX[i] = startAreaXOffset + distanceX * i;
            if(i % 2 == 0){
                //Weist geraden Positionen den Y-Wert "hinten zu"
                positionsY[i] = startAreaY + startAreaYOffset;
            }
            else{
                //Weist ungeraden Positionen den Y-Wert "vorne"
                positionsY[i] = startAreaYOffset;
            }
            printf("Position%d: (%d,%d)\n", i, positionsX[i], positionsY[i]);
        }
        break;
        default:
            printf("Eingabe amountOfPositions ungueltig\n");
            break;
        
    }
} 

void Drive::initializeDriveMotors(){    //Setzt Koordinaten
    printf("initializeDriveMotors\n");

    MotorDriveLeft.setVelocity(0.5f);
    MotorDriveRight.setVelocity(0.5f);

    while(InFrontOfContainer.read() <= triggBeforeContainer); //Fährt solange geradeaus, bis vor Container

    MotorDriveLeft.setVelocity(0.0f);
    MotorDriveRight.setVelocity(0.0f);

    currentPosX = startPosX;
    currentPosY = startAreaYOffset;

    //Berechnet Koordinaten für Perlenaufsammelpositionen
    calculatePositions();
}

void Drive::changeAngleRel(float angle){   //Berechnung in Grad und relativ
    printf("changeAngleRel: %f°\n", angle);

    //Berechnet Winkel zwischen [180, -180] um sich nicht unnötig weit zu drehen
    if(angle > 180.0f){
        angle -= 360.0f;
    }
    else if(angle < -180.0f){
        angle += 360.0f;
    }
    //Berechnung anzahl Umdrehung um Roboter um entsprechenden Winkel zu drehen
    float rotationsRobot = (angle / 360.0f * axialDistance) / wheelDiameter;
    
    MotorDriveLeft.setMaxVelocity(MotorDriveLeft.getMaxPhysicalVelocity() * maxVelocity * 0.25f);
    MotorDriveRight.setMaxVelocity(MotorDriveRight.getMaxPhysicalVelocity() * maxVelocity* 0.25f);
    
    MotorDriveLeft.setRotation(MotorDriveLeft.getRotation() + rotationsRobot);
    MotorDriveRight.setRotation(MotorDriveRight.getRotation() + rotationsRobot);

    MotorDriveLeft.setMaxVelocity(MotorDriveLeft.getMaxPhysicalVelocity() * maxVelocity);
    MotorDriveRight.setMaxVelocity(MotorDriveRight.getMaxPhysicalVelocity() * maxVelocity);

    //Wartet bis Bewegung fertig. Wird gemacht um currentAngle Meldung zu printen, wenn es auch stimmt
    while(MotorDriveLeft.getVoltage() != 0.0f);

    currentAngle += angle; //setzt neuen Winkel

    //Macht das Winkel im Bereich [0°, 360°[ bleibt
    if(currentAngle >= 360.0f){
        currentAngle -= 360.0f;
    }
    else if(currentAngle < 0){
        currentAngle += 360;
    }
}

bool Drive::changeAngleAbs(float angle){   //Berechnung in Rad und absolut
    printf("changeAngleAbs\n");
    //Berechnung relativer Winkel und Unformung in Grad
    float relativeAngle = angle * 180.0f / PI - currentAngle;
    //Falls bereits auf diesem Winkel
    if(relativeAngle > -0.5f && relativeAngle < 0.5f){
        return true;
    }
    else{
        changeAngleRel(relativeAngle);
        return false;
    }
}

void Drive::driveStraight(int distance){
    printf("driveStraight: %d\n", distance);
    //Speichert anzahl umdrehungen für Distanz
    float rotationsDistance = distance / (wheelDiameter * PI);
    MotorDriveLeft.setRotation(MotorDriveLeft.getRotation() + rotationsDistance);
    MotorDriveRight.setRotation(MotorDriveRight.getRotation() + rotationsDistance);
    //Wenn der Roboter das Ziel erreicht hat, werden die Koordinaten ausgerechnet
    while(MotorDriveLeft.getVoltage() != 0.0f);
    //calculateCurrentPos(distance);
}

bool Drive::driveTo(int x, int y, bool direction){
    //Offset für Rückwärtsfahrt um 180°
    float inverseAngle = PI;
    //Lässt Räder in andere Richtung drehen für Rückwärtsfahrt
    int inverseDirection = -1;
    //Ändert fahrtrichtung falls direction = 1 (fährt dann vorwärts)
    if(direction){
        float inverseAngle = 0.0f;
        int inverseDirection = 0;
    }
    // +---> X
    // |
    // v
    // Y
    printf("targetPosition: (%d, %d)\n", x, y);
    int changeInPositionY = currentPosY - y;
    int changeInPositionX = x - currentPosX;

    //Falls Roboter bereits auf Position steht, wird true zurückgegeben
    if(changeInPositionX < 1 && changeInPositionX > -1
    && changeInPositionY < 1 && changeInPositionY > -1){

        return true;
    }
    else{
        //Berechnet Winkel in Rad (Absolut) und dreht sich entsprechend
        if(changeAngleAbs(atan(changeInPositionY / changeInPositionX) + inverseAngle)){
            //Sobald der Roboter mit richtigem Winkel steht, fährt er los
            driveStraight(sqrt(pow(changeInPositionY, 2) + pow(changeInPositionX, 2)) * inverseDirection);
        }
        return false;
    }
}

bool Drive::driveToForwards(int x, int y){
    if(driveTo(x, y, true)){
        return true;
    }
    else{
        return false;
    }
}

bool Drive::driveToBackwards(int x, int y){
    if(driveTo(x, y, false)){
        return true;
    }
    else{
        return false;
    }

}

bool Drive::toTargetContainer(){         //Fährt zum Zielbehälter
    if(driveToBackwards(posTargetContainerX, posTargetContainerY)){
        //Dreht sich vor Zielbehälter so, dass der Behälter in den Zielbehälter ausgeschütet werden kann
        if(changeAngleAbs(PI)){
            return true;
            //Speicher aktuelle Position
            currentPosition = -1;
        }
    }
    else{
        return false;
    }
}

bool Drive::driveToNextPosition(){       //Fährt zur nächsten Position vor dem Startbehälter
    //Flankenerkennen wenn neu Position erreicht wurde
    int oldPosition = currentPosition;
    printf("driveToNextPosition\n");
    //Falls der Roboter auf der Startposition steht
    if(currentPosition == -2){
        switch(amountOfPositions){
            case 1:
                printf("FEHLER: Nur 1 position\n");
                break;
            case 3:
            case 5:
                if(driveToBackwards(positionsX[2], positionsY[2])){
                    currentPosition = 2;
                }
                break;
            case 7:
                if(driveToBackwards(positionsX[4], positionsY[4])){
                    currentPosition = 4;
                }
                break;
        }
    }
    //Falls der Roboter beim Zielbehälter steht
    else if(currentPosition == -1){
        //Fährt zur ersten Aufnahmeposition vor Startbehälter
        if(driveToForwards(positionsX[0], positionsY[0])){
            currentPosition = 0;
        }
    }
    //Falls der Roboter bereits auf einer Position vor dem Startbehälter steht und er weiter aufsammeln muss
    else if(currentPosition >= 0 && currentPosition < amountOfPositions-1){
        //Falls auf Y-Postition "hinten"
        if(currentPosition % 2 == 0){
            if(driveToForwards(positionsX[currentPosition+1], positionsY[currentPosition+1])){
                currentPosition = currentPosition+1;
            }
        }
        //Falls auf Y-Postition "vorne"
        else{
            if(driveToBackwards(positionsX[currentPosition+1], positionsY[currentPosition+1])){
            currentPosition = currentPosition+1;
            }
        }
    }
    else{
        printf("FEHLER: letzte Position erreicht\n");
    }
    if(oldPosition != currentPosition){
        return true;
    }
    else{
        return false;
    }
}

void Drive::calculateCurrentPos(int distance){ //Berechnung in rad
    currentPosX += cos(currentAngle * 180.0f / PI) * distance;
    currentPosY -= sin(currentAngle * 180.0f / PI) * distance;
    printf("currentPos: (%d, %d)\n", currentPosX, currentPosY);

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

int Drive::getPosX(){
    printf("currentPosX: %d\n", currentPosX);
    return currentPosX;
}

int Drive::getPosY(){
    printf("currentPosY: %d\n", currentPosY);
    return currentPosY;
}