#ifndef HEADERS_H
#define HEADERS_H

#include "mbed.h"
#include "pm2_drivers/PESBoardPinMap.h"
#include "pm2_drivers/DebounceIn.h"
#include "DCMotor.h"
#include "pm2_drivers/Servo.h"

class Drive{
    private:
        DCMotor motorDriveRight;
        DCMotor motorDriveLeft;

        //motorDriveRight und motorDriveRight Getriebe
        const float gearRatioMotorDrive = 100.0f;   // gear ratio
        const float rpmV = 140.0f / 12.0f;          // motor constant [rpm/V]
        const float voltageMax = 12.0f;
        const float maxVelocity = 5.14f / voltageMax; // soll 60rpm, 1rps

    public:
        Drive();
        void nextPosition(int pos);       //Fährt zur nächsten Position vor dem Startbehälter, int pos -> welche position vor dem Behälter
        void toStartContainer();          //Fährt zu Startbehälter
        void toFinishContainer();         //Fährt zum Zielbehälter
};

class Mining{
    private:
        DCMotor MotorLiftWheel;
        DigitalIn wheelLowerPosition;

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
        void spinnWheel(bool enable);     //Dreht Schaufelrad, bool enable -> Dreht falls true, Stoppt falls false
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