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
//#define PinInFrontOfContainer PC_2

#define PI 3.1415926f

Drive::Drive() :
    //Erzeugt Motorobjekt
    MotorDriveRight(PinMotorDriveRight, PinEncoderMotorRightA, PinEncoderMotorRightB, 100.0f, 140.0f/12.0f, 12.0f),
    //Erzeugt Motorobjekt
    MotorDriveLeft(PinMotorDriveLeft, PinEncoderMotorLeftA, PinEncoderMotorLeftB, 100.0f, 140.0f/12.0f, 12.0f)
    //Erzeugt AnalogIn
    //InFrontOfContainer(PinInFrontOfContainer)
{                          
    //Aktiviert Motioplanner
    MotorDriveLeft.enableMotionPlanner(true);
    MotorDriveRight.enableMotionPlanner(true);
    // limitiert Maximumgeschwindigkeit
    MotorDriveLeft.setMaxVelocity(MotorDriveLeft.getMaxPhysicalVelocity() * maxVelocity);
    MotorDriveRight.setMaxVelocity(MotorDriveRight.getMaxPhysicalVelocity() * maxVelocity);
}

//Berechnet anhand Konstante amountofPositions die Koordinaten dieser Positionenen
void Drive::calculatePositions(){
    if(amountOfPositions == 1){
        //Weist Position Startwert zu
        positionsX[0] = startPosX;
        positionsY[0] = startAreaYOffset;
        //printf("Position1: (%d, %d)\n", positionsX[0], positionsY[0]);
    }
    else if(amountOfPositions > 1 && amountOfPositions <= 10){
        //Berechnet die Distanz zwischen den einzelnen X-Positionen
        int distanceX = startAreaX / (amountOfPositions-1);
        for(int i = 0; i < amountOfPositions; i++){
            //Weist X-Positionen Koordinate mit gleichem Abstand zu
            positionsX[i] = startAreaXOffset + distanceX * i+1;
            if(i % 2 == 0){
                //Weist geraden Positionen den Y-Wert "vorne"
                positionsY[i] = startAreaYOffset;
            }
            else{
                //Weist ungeraden Positionen den Y-Wert "hinten"
                positionsY[i] = startAreaY + startAreaYOffset;
            }
            //printf("Position%d: (%d, %d)\n", i, positionsX[i], positionsY[i]);
        }
    }
    else{
        //printf("FEHLER: Eingabe amountOfPositions ungueltig\n");
    }
} 

//Initialisiert Position des Roboters mit (Sensor InFrontOfContainer)
bool Drive::initializeDriveMotors(){
    /*
    printf("initializeDriveMotors\n");
    printf("IR voltage: %.2fV", InFrontOfContainer.read());

    MotorDriveLeft.setVelocity(-0.5f);
    MotorDriveRight.setVelocity(0.5f);

    if(InFrontOfContainer.read() >= triggBeforeContainer){ //Fährt solange geradeaus, bis vor Container
        MotorDriveLeft.setVelocity(0.0f);
        MotorDriveRight.setVelocity(0.0f);
    */
        currentPosX = startPosX;
        currentPosY = startPosY;

        return true;
    //}
    //return false;
}

//Ändert Winkel des Roboters (Absolut und in RAD), Rückgabe true wenn Soll-Winkel == Ist-Winkel
bool Drive::changeAngleAbs(float angle){
    /*
    printf("changeAngleAbs\n");
    printf("targetAbsoluteAngle: %.2frad\n", angle);
    printf("currentAngle: %.1f°\n", currentAngle);
    printf("rotation: %f\n", MotorDriveLeft.getRotation());
    printf("targetRotationsAngle: %f\n", targetRotationsAngle);
    */
    //Berechnung relativer Winkel und Unformung in Grad
    float relativeAngle = angle * 180.0f / PI - currentAngle;
    
    //Falls bereits auf diesem Winkel
    if(equalTo(relativeAngle, 0) && !isChangingAngle){
        return true;
    }//Winkel wurde gerade geändert
    else if(equalTo(MotorDriveLeft.getRotation(), targetRotationsAngle) && isChangingAngle && equalTo(relativeAngle, 0)){
        //Setzt nach Drehung wieder auf Standardgeschwindigkeit
        MotorDriveLeft.setMaxVelocity(MotorDriveLeft.getMaxPhysicalVelocity() * maxVelocity);
        MotorDriveRight.setMaxVelocity(MotorDriveRight.getMaxPhysicalVelocity() * maxVelocity);
        isChangingAngle = false;
        return true;
    }
    //Falls bereits am drehen
    else if(isChangingAngle){
        return false;
    }
    //Lässt für drehung Motoren nur auf 40% der Standardgeschwindigkeit fahren
    MotorDriveLeft.setMaxVelocity(MotorDriveLeft.getMaxPhysicalVelocity() * maxVelocity * 0.4f);
    MotorDriveRight.setMaxVelocity(MotorDriveRight.getMaxPhysicalVelocity() * maxVelocity* 0.4f);
    changeAngleRel(relativeAngle);
    isChangingAngle = true;
    return false;
}

//Ändert Winkel des Roboters (Relativ und in GRAD)
void Drive::changeAngleRel(float angle){
    //printf("changeAngleRel: %.1f°\n", angle);
    //Berechnet Winkel zwischen [180, -180] um sich nicht unnötig weit zu drehen
    if(angle > 180.0f){
        angle -= 360.0f;
    }
    else if(angle < -180.0f){
        angle += 360.0f;
    }
    //Berechnung anzahl Umdrehung um Roboter um entsprechenden Winkel zu drehen
    float rotationsRobot = (angle / 360.0f * axialDistance) / wheelDiameter;

    targetRotationsAngle = MotorDriveLeft.getRotation() + rotationsRobot;

    MotorDriveLeft.setRotation(targetRotationsAngle);
    MotorDriveRight.setRotation(MotorDriveRight.getRotation() + rotationsRobot);

    currentAngle += angle; //setzt neuen Winkel

    //Macht das Winkel im Bereich [0°, 360°[ bleibt
    if(currentAngle >= 360.0f){
        currentAngle -= 360.0f;
    }
    else if(currentAngle < 0){
        currentAngle += 360;
    }
}

//Wie weit der Roboter geradeaus fahren soll (Relativ)
bool driveStraight(int distance);
bool Drive::driveStraight(int distance){
    //printf("driveStraight distance: %d\n", distance);
    //Falls driveStraight noch nicht fährt
    if(!isDriving){
        //Speichert anzahl umdrehungen für Distanz
        float rotationsDistance = distance / (wheelDiameter * PI);
        targetRotationsDriveStraight = MotorDriveLeft.getRotation() - rotationsDistance;
        MotorDriveLeft.setRotation(targetRotationsDriveStraight);
        MotorDriveRight.setRotation(MotorDriveRight.getRotation() + rotationsDistance);
        isDriving = true;
        return false;
    //Falls driveStraight gerade angekommen ist
    }
    else if(equalTo(MotorDriveLeft.getRotation(), targetRotationsDriveStraight) && isDriving){
        isDriving = false;
        return true;
    }
    return false;
}

//Fährt Koordinaten an, Rückgabe true wenn bereits auf diesen Koordinate, direction 1 = geradeaus, 0 = rückwärts anfahren
bool Drive::driveTo(int x, int y, bool direction){
    //Offset für Rückwärtsfahrt um 180°
    float inverseAngle = PI;
    //Lässt Räder in andere Richtung drehen für Rückwärtsfahrt
    int inverseDirection = -1;
    //Ändert fahrtrichtung falls direction = 1 (fährt dann vorwärts)
    if(direction){
        inverseAngle = 0.0f;
        inverseDirection = 1;
    }
    //printf("currentPosition: (%d, %d)\n", currentPosX, currentPosY);
    //printf("targetPosition: (%d, %d)\n", x, y);
    int changeInPositionY = currentPosY - y;
    int changeInPositionX = x - currentPosX;

    //Falls Roboter bereits auf Position steht, wird true zurückgegeben
    if(changeInPositionX == 0 && changeInPositionY == 0){
        return true;
    }
    else{
        //Berechnet Winkel in Rad (Absolut) und dreht sich entsprechend
        if(changeAngleAbs(atan2(changeInPositionY, changeInPositionX) + inverseAngle)){

            //Sobald der Roboter mit richtigem Winkel steht, fährt er los
            if(driveStraight(sqrt(pow(changeInPositionY, 2) + pow(changeInPositionX, 2)) * inverseDirection)){
                currentPosX = x;
                currentPosY = y;
                return true;
            }
        }
    }
    return false;
}

//Fährt geradeaus auf Zielkoordinaten, Rückgabewert true, falls bereits dort
bool Drive::driveToForwards(int x, int y){
    //printf("driveToForwards\n");
    if(driveTo(x, y, true)){
        return true;
    }
    return false;
}

//Fährt rückwärts auf Zielkoordinaten, Rückgabewert true, falls bereits dort
bool Drive::driveToBackwards(int x, int y){
    //printf("driveToBackwards\n");
    if(driveTo(x, y, false)){
        return true;
    }
    return false;
}

bool Drive::driveInFrontOfPos(){
    if(driveToForwards(positionsX[deletedPositions + 1], posTargetContainerY - 50)){
        return true;
    }
    return false;
}

//Ändert aktuelle Position umd die der Paramter. Fährt vorwärts falls direction == true
bool Drive::driveRelative(int x, int y, bool direction){ //direction = true -> fährt vorwärts
    if(direction){
        if(driveToForwards(currentPosX + x, currentPosY + y)){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        if(driveToBackwards(currentPosX + x, currentPosY + y)){
            return true;
        }
        else{
            return false;
            }
    }
}

//Fährt zum Zielbehälter und dreht sich auf Absolut 90°
bool Drive::toTargetContainer(){         //Fährt zum Zielbehälter
    //printf("toTargetContainer\n");
    if(driveToBackwards(posTargetContainerX, posTargetContainerY)){
        //Dreht sich vor Zielbehälter so, dass der Behälter in den Zielbehälter ausgeschütet werden kann
        if(changeAngleAbs(PI)){
            return true;
            //Speicher aktuelle Position
            currentPosition = -1;
        }
    }
    return false;
}

//Fährt zur nächsten Position vor dem Startbehälter, Rückgabewert true Flanke, wenn dort angekommen.
bool Drive::driveToNextPosition(){       //Fährt zur nächsten Position vor dem Startbehälter
    //Flankenerkennen wenn neu Position erreicht wurde
    int oldPosition = currentPosition;
    //printf("driveToNextPosition\n");
    //printf("currentPos: %d\n", currentPosition);
    //Falls der Roboter bereits auf einer Position vor dem Startbehälter steht und er weiter aufsammeln muss
    if(currentPosition < amountOfPositions-1){
        //Falls auf Y-Postition "vorne"
        if(currentPosition % 2 == 0){
            if(driveToBackwards(positionsX[currentPosition+1], positionsY[currentPosition+1])){
                currentPosition += 1;
            }
        }
        //Falls auf Y-Postition "hinten"
        else{
            if(driveToForwards(positionsX[currentPosition+1], positionsY[currentPosition+1])){
                currentPosition += 1;
            }
        }
    }
    else{
        //printf("Letzte Position erreicht\n");
    }
    if(oldPosition != currentPosition && currentPosition %2 == 0){
        return true;
    }
    return false;
}

//Gibt true zurück, falls es keine weiteren Aufsammelpositionen mehr gibt
bool Drive::lastPositionReached(){
    if(currentPosition == amountOfPositions - 1){
        return true;
    }
    return false;
}

 //Löscht aktuelle und niedrigere Positionen, bei denen bereits Perlen aufgesammelt wurden
void Drive::deleteCurrentPos(){ //Löscht aktuelle und niedrigere Positionen, bei denen bereits Perlen aufgesammelt wurden
    deletedPositions = currentPosition + 1;
    //Falls alle Positionen bereits angefahren wurden, wird als nächstes wieder Pos0 angefahren (reset der deletedPositions)
    if(deletedPositions >= amountOfPositions - 1){
        deletedPositions = -1;
    }
}

//Vergleicht Soll und Zielwerte mit einer Tolerenz und gibt true zurück falls sie gleich sind
bool Drive::equalTo(float value1, float value2){
    if(value1 > value2 - rotationTolerance && value1 < value2 + rotationTolerance){
        return true;
    }
    return false;
}

//Für Tests

/*
float Drive::getIrSensor(){
    //printf("InFrontOfContainer: %f\n", InFrontOfContainer.read());
    return InFrontOfContainer.read();
}
*/

float Drive::rotateRightWheel(){
    MotorDriveRight.setVelocity(-1.0f);
    //printf("MotorDriveRight voltage: %.1f\n", MotorDriveRight.getVoltage());
    return MotorDriveRight.getVoltage();
}

float Drive::rotateLeftWheel(){
    MotorDriveLeft.setVelocity(1.0f);
    //printf("MotorDriveRLeft voltage: %.1f\n", MotorDriveLeft.getVoltage());
    return MotorDriveLeft.getVoltage();
}