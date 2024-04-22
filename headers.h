#ifndef HEADERS_H
#define HEADERS_H

#include "mbed.h"
#include "pm2_drivers/PESBoardPinMap.h"
#include "pm2_drivers/DebounceIn.h"
#include "DCMotor.h"
#include "pm2_drivers/Servo.h"

class Drive{
    private:
        //Rechter Fahrantrieb
        DCMotor MotorDriveRight;
        //Linker Fahrantrieb
        DCMotor MotorDriveLeft;
        //Optischer Sensor
        AnalogIn InFrontOfContainer;

        //MotorDriveRight und MotorDriveLeft Parameter
        const float gearRatioMotorDrive = 100.0f;   // Getriebe
        const float rpmV = 140.0f / 12.0f;          // [rpm/V]
        const float voltageMax = 12.0f;             //Maximalspannung
        const float maxVelocity = 5.14f / voltageMax; // soll 60rpm, 1rps

        const float axialDistance = 150.0f; //Abstand der beiden Räder in mm
        const float wheelDiameter = 43.0f;  //Durchmesser der Antriebsräder in mm

        float currentAngle = 90.0f; //Speichert absoluten Winkel in Grad

        int positionsX[7] = {0};    //Speichert X Koordinaten der Aufnahme-Positionen des Roboters vor dem StartContainer, wird in calculatePositions berechnet
        int positionsY[7] = {0};
        const int startAreaX = 180; //Wie breit der Bereich vor dem Startcontainer ist, wo sich der Roboter hinstellen darf
        const int startAreaY = 50;
        const int startAreaXOffset = (250 - startAreaX) / 2; //Abstand der StartAreaX zum Koordinatenursprung (0,0) bei der linken unteren Ecke des Startbehälters
        const int startAreaYOffset = 85;
        const int startPosX = 125; //Wieviele mm vor dem Startbehälter sich die Räder befinden beim Initialisieren (Rädermittelpunkt ist Koordinatenursprung von Roboter)
        const float triggBeforeContainer = 2.5f / 3.3f; //2.5V Spannung, wird normiert. Schwellwert ab wann der Roboter vor dem Container steht (bezogen auf Sensor)
        const int posTargetContainerX = 1025-150; //Koordinaten wohin der Roboter beim anfahren vom Zielbehälter hinfährt (mit Offset)
        const int posTargetContainerY = 250;

        int currentPosX; //Speicher aktuelle X position
        int currentPosY; //Speichert aktuelle Y position

        const int amountOfPositions = 3; //Wieviele verschiedene Aufnahmepositionen vor Startbehälter es gibt, braucht es, um bestmögliche Verteilung vor Startbehählter zu berechnen
                                         // Mögliche Werte sind: 1,3,5,7
        int currentPosition = -2;   //Speichert auf welcher Position beim Perlen aufnehmen sich der Roboter aktuell befindet
                                    //-2: Startposition vor Container
                                    //-1: Zielcontainer
                                    //0-6: Vor Startcontainer, Sammelt Perlen auf

        void calculateCurrentPos(int distance);     //Berechnet aktuelle Koordinaten, wenn vorhin über changeAngleRel und driveStraight gefahren wurde
        void calculatePositions();                  //Berechnet anhand Konstante amountofPositions die Koordinaten dieser Positionenen
        bool driveTo(int x, int y, bool direction); //Fährt Koordinaten an, Rückgabe true wenn bereits auf diesen Koordinate, direction 1 = geradeaus, 0 = rückwärts anfahren
        bool driveToForwards(int x, int y);         //Fährt geradeaus auf Zielkoordinaten, Rückgabewert true, falls bereits dort
        bool driveToBackwards(int x, int y);        //Fährt rückwärts auf Zielkoordinaten, Rückgabewert true, falls bereits dort
        void changeAngleRel(float angle);           //Ändert Winkel des Roboters (Relativ und in GRAD)
        bool changeAngleAbs(float angle);           //Ändert Winkel des Roboters (Absolut und in RAD), Rückgabe true wenn soll Winkel = ist Winkel
        void driveStraight(int distance);           //Wie weit der Roboter geradeaus fahren soll (Relativ)

    public:
        Drive();
        void initializeDriveMotors();               //Initialisiert Position des Roboters mit Sensor InFrontOfContainer und ruft calculatepositions auf
        bool driveToNextPosition();                 //Fährt zur nächsten Position vor dem Startbehälter, Rückgabewert true Flanke, wenn dort angekommen
        bool toTargetContainer();                   //Fährt zum Zielbehälter und dreht sich auf Absolut 90°
        int getPosX();                              //Gibt aktuelle X Position zurück
        int getPosY();                              //Gibt aktuelle Y Position zurück
};

class Mining{
    private:
        DCMotor MotorLiftWheel;
        DigitalIn WheelLowerPosition;
        DigitalOut MotorWheel;

        const float gearRatioMotorLiftWheel = 31.25f;
        const float rpmV = 450.0f / 12.0f;
        const float voltageMax = 12.0f;
        const float maxVelocity = 1.0f;
        const float standardVelocity = 8.13f / voltageMax; // soll 350rpm, 5rps

        const float ThreadPitch = 1.25f; //Steigung des Gewinde
        const float threadedRodlengt = 80.0f; //Länge der Gewindestange
        const float wheel10cmRod = 45.0f; //auf welchecher Position (mm auf der Gewindestange) sich der Motor befindet, um über den Perlen zu sein

        const float wheelUpperPosRotation = threadedRodlengt / ThreadPitch; //Anzahl Umdreungen bis ganz oben
        const float wheel10cmPosRotation = wheel10cmRod / ThreadPitch; //Anzahl Umdrehung bis auf höhe Perlen

        float wheelLowerPositionRotation = 0; //Hier wird nach dem Nullen der Offset zum Nullpunkt gespeichert
        float wheelUpperPosRotationOff = wheelUpperPosRotation + wheelLowerPositionRotation; //Anzahl Umdreungen bis ganz oben (mit offset)
        float wheel10cmPosRotationOff = wheel10cmPosRotation + wheelLowerPositionRotation; //Anzahl Umdrehung bis auf höhe Perlen (mit offset)

        bool equalTo(float value1, float value2); //Vergleicht Soll und Zielwerte mit Tolerenz und gibt 1 zurück falls sie gleich sind
        
    public:
        Mining();
        void initializeMotorLiftWheel();  //Nullt den Encouder des Motors MotorLiftWheel
        void spinWheel(bool enable);     //Dreht Schaufelrad, bool enable -> Dreht falls true, Stoppt falls false
        bool WheelToUpperPos();           //Hebt Schaufelrad in die obere Endlage   (Volle Geschwindigkeit)
        bool WheelTo10cm();               //Senkt Schaufelrad auf 10cm    (Volle Geschwindigkeit)
        bool lowerWheel();                //Senkt Schaufelrad, rückgabewert false wenn ganz unten
};

class Container{
    private:
        Servo ServoTilt;

        const float servoTiltAngleMin = 0.0150f;
        const float servoTiltAngleMax = 0.1150f;

    public:
        Container();
        void tiltContainer(bool enable);   //Kippt Behälter, bool enable -> Kippt falls true
};

#endif