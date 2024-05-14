#ifndef HEADERS_H
#define HEADERS_H

#include "mbed.h"
#include "pm2_drivers/PESBoardPinMap.h"
#include "pm2_drivers/DebounceIn.h"
#include "DCMotor.h"
#include "pm2_drivers/Servo.h"
#include "UltrasonicSensor.h"

class Drive{
    private:
        //Rechter Fahrantrieb
        DCMotor MotorDriveRight;
        //Linker Fahrantrieb
        DCMotor MotorDriveLeft;
        //Optischer Sensor
        AnalogIn InFrontOfContainer;

        //MotorDriveRight und MotorDriveLeft Parameter
        const float maxVelocity = 5.0f / 12.0f;   //soll 60rpm, 1rps

        const float axialDistance = 175.0f; //Abstand der beiden Räder in mm
        const float wheelDiameter = 45.4f;  //Durchmesser der Antriebsräder in mm

        float currentAngle = 90.0f;          //Speichert absoluten Winkel in Grad
        float targetRotationsAngle = 0.0f;   //Speichert Zielumdrehungen für changeAngleAbs
        bool isChangingAngle = false;        //Speichert ob der Roboter gerade den Winkel ändert

        float targetRotationsDriveStraight = 0.0f;  //Speichert Zielumdrehungen für driveStraight
        
        //Koordinatensystem:
        // +---> X
        // |
        // v
        // Y
        
        const int startAreaX = 250 - axialDistance;             //Wie breit der Bereich vor dem Startcontainer ist, wo sich der Roboter hinstellen darf
        const int startAreaY = 25;
        const int startAreaXOffset = (250 - startAreaX) / 2;    //Abstand der StartAreaX zum Koordinatenursprung (0,0) bei der linken unteren Ecke des Startbehälters (Zentriert)
        const int startAreaYOffset = 30;                        
        const int startPosY = 172.0f;                           //Wieviele mm vor dem Startbehälter sich die Räder befinden beim Initialisieren (Rädermittelpunkt ist Koordinatenursprung von Roboter) befinden
        const int startPosX = axialDistance / 2;                //Wieviele mm rechts vom Koordinatenursprung sich die Räder befinden beim Initialisieren (Rädermittelpunkt ist Koordinatenursprung von Roboter) befinden
        
        //const float triggBeforeContainer = 1.7f / 3.3f;         //Spannung, wird normiert. Schwellwert ab wann der Roboter vor dem Container steht (bezogen auf Sensor)

        bool isDriving = false; //Flankenerkennung ob Roboter gerade am fahren ist

        const int posTargetContainerX = 1025-180; //Koordinaten wohin der Roboter beim anfahren vom Zielbehälter hinfährt (mit Offset)
        const int posTargetContainerY = 250;

        int currentPosX; //Speicher aktuelle X position
        int currentPosY; //Speichert aktuelle Y position

        int positionsX[10] = {0};    //Speichert X Koordinaten der Aufnahme-Positionen des Roboters vor dem StartContainer, wird in calculatePositions berechnet
        int positionsY[10] = {0};

        const int amountOfPositions = 5;    //Wieviele verschiedene Aufnahmepositionen vor Startbehälter es gibt, braucht es, um bestmögliche Verteilung vor Startbehälter zu berechnen
                                            //Max. 10
        int currentPosition = 0;    //Speichert auf welcher Position beim Perlen aufnehmen sich der Roboter aktuell befindet
                                    //-1: Zielcontainer
                                    //0-6: Vor Startcontainer, Sammelt Perlen auf
        int deletedPositions = -1;  //Speichert welche Positionen nicht mehr angefahren wird
                                    //-1: alle Positionen sind verfügbar
                                    //0: Position0 ist nicht mehr verfügbar
                                    //1: ...
                                    
        bool changeAngleAbs(float angle);           //Ändert Winkel des Roboters (Absolut und in RAD), Rückgabe true wenn soll Winkel = ist Winkel
        void changeAngleRel(float angle);           //Ändert Winkel des Roboters (Relativ und in GRAD)
        bool driveStraight(int distance);           //Wie weit der Roboter geradeaus fahren soll (Relativ)
        bool driveTo(int x, int y, bool direction); //Fährt Koordinaten an, Rückgabe true wenn bereits auf diesen Koordinate, direction 1 = geradeaus, 0 = rückwärts anfahren
        bool driveToForwards(int x, int y);         //Fährt geradeaus auf Zielkoordinaten, Rückgabewert true, falls bereits dort
        bool driveToBackwards(int x, int y);        //Fährt rückwärts auf Zielkoordinaten, Rückgabewert true, falls bereits dort
        bool equalTo(float value1, float value2);   //Vergleicht Soll und Zielwerte mit 0.01 Tolerenz und gibt true zurück falls sie gleich sind
    
    public:
        Drive();               
        bool initializeDriveMotors();               //Initialisiert Position des Roboters mit Sensor InFrontOfContainer und ruft calculatepositions auf
        void calculatePositions();                  //Berechnet anhand Konstante amountofPositions die Koordinaten dieser Positionenen
        bool driveToNextPosition();                 //Fährt zur nächsten Position vor dem Startbehälter, Rückgabewert true Flanke, wenn dort angekommen
        bool toTargetContainer();                   //Fährt zum Zielbehälter und dreht sich auf Absolut 90°
        void deleteCurrentPos();                    //Löscht aktuelle und niedrigere Positionen, bei denen bereits Perlen aufgesammelt wurden

        //Für Tests
        float getIrSensor();
        float rotateRightWheel();
        float rotateLeftWheel();
};

class Mining{
    private:
        //Antrieb um Schaufelrad zu heben
        DCMotor MotorLiftWheel;
        //Mechanischer Endschalter um höhenantrieb zu nullen
        DigitalIn WheelLowerPosition;
        //Ausgang um Schaufelrad anzusteuern
        DigitalOut MotorWheel;

        const float maxVelocityRps = 6.0f; //7.5f
        const float standardVelocity = 4.0f; //5.0f

        const float maxVelocity = maxVelocityRps / 12.0f;

        const float ThreadPitch = 1.5f;         //Steigung der Gewindestange
        const float threadedRodlengt = 95.0f;   //Länge der Gewindestange
        const float wheel10cmRod = 80.0f;       //auf welchecher Position (mm auf der Gewindestange) sich der Motor befindet, um über den Perlen zu sein (Theoretisch 10cm über Boden)

        const float wheelUpperPosRotation = threadedRodlengt / ThreadPitch; //Anzahl Umdreungen, die der Motor braucht bis das Schaufelrad in der oberen Endlage ist
        const float wheel10cmPosRotation = wheel10cmRod / ThreadPitch;      //Anzahl Umdreungen, die der Motor braucht bis das Schaufelrad auf 10 cm ist

        float wheelLowerPositionRotation = 0;   //Hier wird nach dem Nullen der Offset zum Nullpunkt gespeichert
        float wheelUpperPosRotationOff = 0;     //Anzahl Umdreungen bis ganz oben (mit offset)
        float wheel10cmPosRotationOff = 0;      //Anzahl Umdrehung bis auf höhe Perlen (mit offset)
    
        bool equalTo(float value1, float value2);   //Vergleicht Soll und Zielwerte mit 0.05 Tolerenz und gibt true zurück falls sie gleich sind
        void printMotorLiftPos();                   //gibt die aktuelle höhe (in mm) auf der Sich der Arm befindet auf den Serialmonitor aus
        
    public:
        Mining();
        bool initializeMotorLiftWheel();            //Nullt den Encouder des Motors MotorLiftWheel, true wenn auf Endschalter
        void spinWheel(bool enable);                //Dreht Schaufelrad, bool enable -> Dreht falls true, Stoppt falls false
        bool wheelToUpperPos();                     //Hebt Schaufelrad in die obere Endlage   (Volle Geschwindigkeit)
        bool wheelTo10cm();                         //Senkt Schaufelrad auf 10cm    (Volle Geschwindigkeit)
        bool lowerWheel();                          //Senkt Schaufelrad, rückgabewert true wenn ganz unten

        //Für Tests
        float liftWheel();
        bool getMechanicalSwitch();
};

class Container{
    private:
        //Ultraschallsensor um zu erkennen, ob der Behälter voll ist
        UltrasonicSensor ContainerFull;
        //Servo um Behälter auskippen zu können
        Servo ServoTilt;

        const float servoTiltAngleMin = 0.0150f;
        const float servoTiltAngleMax = 0.1150f;
        const float triggContainerFull = 5.0f; //Bei welchem Abstand zum Ultraschallsensor in cm der Container als voll gilt

    public:
        Container();
        bool containerFull();               //Gibt true zurück, falls behälter voll ist
        void tiltContainer(bool enable);    //Kippt Behälter, bei true
};

#endif