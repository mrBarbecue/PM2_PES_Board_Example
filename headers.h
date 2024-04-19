#ifndef HEADERS_H
#define HEADERS_H

#include "mbed.h"
#include "pm2_drivers/PESBoardPinMap.h"
#include "pm2_drivers/DebounceIn.h"
#include "DCMotor.h"
#include "pm2_drivers/Servo.h"

class Drive{
    //Navigiert duch Winkel und gefahrene Strecke (rotations)
    private:
        DCMotor motorDriveRight;
        DCMotor motorDriveLeft;
        AnalogIn beforeContainer;

        //motorDriveRight und motorDriveRight Getriebe
        const float gearRatioMotorDrive = 100.0f;   // gear ratio
        const float rpmV = 140.0f / 12.0f;          // motor constant [rpm/V]
        const float voltageMax = 12.0f;
        const float maxVelocity = 5.14f / voltageMax; // soll 60rpm, 1rps

        const float axialDistance = 150.0f; //Abstand der beiden Räder in mm
        const float wheelDiameter = 43.0f; //Durchmesser der Antriebsräder

        float lastAngle = 0.0f; //Speichert letzten veränderten Winkel des Roboters
        float currentAngle = 90.0f; //Speichert absoluten Winkel in Grad
        float rotationsRobot = 0; //Speichert Anzahl Umdrehungen um Roboter um entsprechenden Winkel zu drehen
        float rotationsDistance = 0; //Speichert anzahl umdrehungen für Distanz

        const int startAreaX = 250; //In milimeter, wie breit der Bereich vor dem Startcontainer ist, wo sich der Roboter hinstellen kann
        const int startAreaY = 80;
        const int startPosX = 125; //Wieviele mm vor dem Startbehälter sich die Räder befinden (Rädermittelpunkt ist Koordinatenursprung von Roboter)
        const int startPosY = 150; //Ebenfalls bezogen auf Räder
        const float triggBeforeContainer = 2.5f / 3.3f; //2.5V Spannung, wird normiert. Schwellwert ab wann der Roboter vor dem Container steht (bezogen auf Sensor)

        float currentPosX; //Speicher aktuelle X position
        float currentPosY; //Speichert aktuelle Y position

        const int amountOfPositions = 4; //Wieviele verschiedene Aufnahmepositionen vor Startbehälter es gibt, braucht es, um bestmögliche Verteilung vor Startbehählter zu berechnen
        int currentPosition = 0; //Speichert wo sich der Roboter aktuell befindet (Nicht Koordinaten)

    private:
        void changeAngleRel(int angle);        //Ändert Winkel des Roboters (Relativ und in GRAD)
        void changeAngleAbs(int angle);        //Ändert Winkel des Roboters (Absolut und in RAD)
        void driveStraight(int distance);   //Wie weit der Roboter gerade fahren soll (Relativ)
        void calculatePos(int distance);    //Berechnet aktuelle Koordinaten, wenn vorhin über changeAngle und driveStraight gefahren wurde
        void getPosX();                     //Gibt aktuelle X Position zurück
        void getPosY():                     //Gibt aktuelle Y Position zurück

    public:
        Drive();
        void initializeDriveMotors();      
        void driveTo(int x, int y);       //Fährt Koordinaten an
        void nextPosition(int pos);       //Fährt zur nächsten Position vor dem Startbehälter, int pos -> welche position vor dem Behälter
        void toStartContainer();          //Fährt zu Startbehälter
        void toFinishContainer();         //Fährt zum Zielbehälter
};

class Mining{
    private:
        DCMotor MotorLiftWheel;
        DigitalIn wheelLowerPosition;
        DigitalOut motorWheel;

        const float gearRatioMotorLiftWheel = 31.25;
        const float rpmV = 450.0f / 12.0f;
        const float voltageMax = 12.0f;
        const float maxVelocity = 1.0f;
        const float standardVelocity = 8.13f / voltageMax; // soll 350rpm, 5rps
        const float ThreadPitch = 1.25f; //Steigung des Gewinde
        const float threadedRodlengtMm = 80.0f; //Länge der Gewindestange
        const float wheel10cmRod = 45.0f; //auf welchecher Position (mm auf der Gewindestange) sich der Motor befindet, um über den Perlen zu sein

        const float wheelUpperPosRotation = threadedRodlengtMm / ThreadPitch; //Anzahl Umdreungen bis ganz oben
        const float wheel10cmPosRotation = wheel10cmRod / ThreadPitch; //Anzahl Umdrehung bis auf höhe Perlen

        float wheelLowerPositionRotation = 0; //Hier wird nach dem Nullen der Nullpunkt gespeichert
        float wheelUpperPosRotationOff = wheelUpperPosRotation + wheelLowerPositionRotation; //Anzahl Umdreungen bis ganz oben (mit offset)
        float wheel10cmPosRotationOff = wheel10cmPosRotation + wheelLowerPositionRotation; //Anzahl Umdrehung bis auf höhe Perlen (mit offset)


    public:
        Mining();
        void spinWheel(bool enable);     //Dreht Schaufelrad, bool enable -> Dreht falls true, Stoppt falls false
        void initializeMotorLiftWheel();  //Nullt den Encouder des Motors MotorLiftWheel
        bool lowerWheel();                //Senkt Schaufelrad, rückgabewert false wenn ganz unten
        void WheelToUpperPos();           //Hebt Schaufelrad in die obere Endlage   (Volle Geschwindigkeit)
        void WheelTo10cm();               //Senkt Schaufelrad auf 10cm    (Volle Geschwindigkeit)
};

class Container{
    private:
        Servo servoTilt;

        const float servoTiltAngleMin = 0.0150f;
        const float servoTiltAngleMax = 0.1150f;

    public:
        Container();
        void tiltContainer(bool enable);   //Kippt Behälter, bool enable -> Kippt falls true
};

#endif