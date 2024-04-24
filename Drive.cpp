#include "headers.h"
//Pins an denen die beiden Motoren angeschlossen sind
#define PinMotorDriveRight PB_PWM_M1
#define PinMotorDriveLeft PB_PWM_M2
#define PinEncoderMotorRightA PB_ENC_A_M1
#define PinEncoderMotorRightB PB_ENC_B_M1
#define PinEncoderMotorLeftA PB_ENC_A_M2
#define PinEncoderMotorLeftB PB_ENC_B_M2

//Pin an dem der optische Sensor zum erkennen ob der Roboter vor dem Container
//steht angeschlossen ist
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
    //Am anfang sind motionplanner für die Winkelinitialisierung ausgeschaltet
    MotorDriveLeft.enableMotionPlanner(false);
    MotorDriveRight.enableMotionPlanner(false);
    // limitiert Maximumgeschwindigkeit
    MotorDriveLeft.setMaxVelocity(MotorDriveLeft.getMaxPhysicalVelocity() * maxVelocity);
    MotorDriveRight.setMaxVelocity(MotorDriveRight.getMaxPhysicalVelocity() * maxVelocity);
}

void Drive::calculatePositions(){
    bool calculatePos = false; //wird bei 3,5 und 7 true gesetzt um in diesen Fällen die Positionen zu berechnen
    switch(amountOfPositions){
        case 1:
            currentPosition = 0;
            //Weist Position Startwert zu
            positionsX[0] = startPosX;
            positionsY[0] = startAreaYOffset;
            printf("Position1: (%d,%d)\n", positionsX[0], positionsY[0]);
            break;
        case 3:
            currentPosition = 1;
            calculatePos = true;
            break;
        case 5:
            currentPosition = 2;
            calculatePos = true;
            break;
        case 7:
            currentPosition = 3;
            calculatePos = true;
            break;
        default:
            printf("Eingabe amountOfPositions ungueltig\n");
            break;
    }
    if(calculatePos){
        //Berechnet die Distanz zwischen den einzelnen X-Positionen
        int distanceX = startAreaX / (amountOfPositions-1);
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
    }
} 

bool Drive::initializeAngle(){  //Stellt den Roboter Senkrecht zum Startbehälter und initialisiert den Winkel
    printf("initializeAngle\n");
    //Lässt roboter langsam nach rechts drehen
    MotorDriveLeft.setVelocity(-0.3f);
    MotorDriveRight.setVelocity(-0.3f);
    //Wenn der neue Wert beim einlesen 3 mal hintereinander grösser ist, wird die Schlaufe verlassen
    //Stellt sicher, dass nicht ein einziger lesefehler/Toleranz die Messung unggenau machen kann
    if(InFrontOfContainer.read() > maxIrValue){
        maxIrValue = InFrontOfContainer.read();
        return false;
    }
    else{
        return true;
    }
}

void Drive::angleInitialized(){ //Wird aufgerufen wenn der Winkel initialisiert wurde und setzt entsprechend die Variable
    MotorDriveLeft.setVelocity(0.0f);
    MotorDriveRight.setVelocity(0.0f);
    MotorDriveLeft.enableMotionPlanner(true);
    MotorDriveRight.enableMotionPlanner(true);

    currentAngle = 90.0f + angleOffset;
}

bool Drive::initializeDriveMotors(){    //Setzt Koordinaten
    printf("initializeDriveMotors\n");

    MotorDriveLeft.setVelocity(-0.5f);
    MotorDriveRight.setVelocity(0.5f);

    if(InFrontOfContainer.read() >= triggBeforeContainer){ //Fährt solange geradeaus, bis vor Container
        MotorDriveLeft.setVelocity(0.0f);
        MotorDriveRight.setVelocity(0.0f);

        currentPosX = startPosX;
        currentPosY = startAreaYOffset;

        return true;
    }
    else{
        return false;
    }
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

    MotorDriveLeft.setMaxVelocity(MotorDriveLeft.getMaxPhysicalVelocity() * maxVelocity * 0.5f);
    MotorDriveRight.setMaxVelocity(MotorDriveRight.getMaxPhysicalVelocity() * maxVelocity* 0.5f);
    
    MotorDriveLeft.setRotation(MotorDriveLeft.getRotation() + rotationsRobot);
    MotorDriveRight.setRotation(MotorDriveRight.getRotation() + rotationsRobot);

    MotorDriveLeft.setMaxVelocity(MotorDriveLeft.getMaxPhysicalVelocity() * maxVelocity);
    MotorDriveRight.setMaxVelocity(MotorDriveRight.getMaxPhysicalVelocity() * maxVelocity);


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
    if(relativeAngle > -0.5f && relativeAngle < 0.5f && MotorDriveLeft.getVoltage() == 0.0f){
        return true;
    }
    else{
        changeAngleRel(relativeAngle);
        return false;
    }
}

bool Drive::driveStraight(int distance){
    printf("driveStraight: %d\n", distance);
    //Speichert anzahl umdrehungen für Distanz
    printf("driveStraightVoltage: %f\n", MotorDriveLeft.getVoltage());
    //Falls die Motoren gerade nicht angesteuert sind und driveStraight auch nicht gerade angekommen ist
    if(MotorDriveLeft.getVoltage() == 0.0f && !isDriving){
        float rotationsDistance = distance / (wheelDiameter * PI);
        MotorDriveLeft.setRotation(MotorDriveLeft.getRotation() - rotationsDistance);
        MotorDriveRight.setRotation(MotorDriveRight.getRotation() + rotationsDistance);
        isDriving = true;
        return false;
    }
    else if(MotorDriveLeft.getVoltage() == 0.0f && isDriving){
        isDriving = false;
        return true;
    }
    else{
        return false;
    }
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
            if(driveStraight(sqrt(pow(changeInPositionY, 2) + pow(changeInPositionX, 2)) * inverseDirection)){
                currentPosX = x;
                currentPosY = y;
            }
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
    //Falls der Roboter beim Zielbehälter steht
    if(currentPosition == -1){
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
                currentPosition += 1;
            }
        }
        //Falls auf Y-Postition "vorne"
        else{
            if(driveToBackwards(positionsX[currentPosition+1], positionsY[currentPosition+1])){
                currentPosition += 1;
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