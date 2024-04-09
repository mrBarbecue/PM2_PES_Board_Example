#ifndef HEADERS_H
#define HEADERS_H

#include "mbed.h"
#include "pm2_drivers/PESBoardPinMap.h"
#include "pm2_drivers/DebounceIn.h"
#include "DCMotor.h"
#include "pm2_drivers/Servo.h"


//drive.cpp
void nextPosition(int pos);       //Fährt zur nächsten Position vor dem Startbehälter, int pos -> welche position vor dem Behälter
void toStartContainer();          //Fährt zu Startbehälter
void toFinishContainer();         //Fährt zum Zielbehälter

//mining.cpp
void spinnWheel(int enable);      //Dreht Schaufelrad, int enable -> Dreht falls true, Stoppt falls false
void initializeMotorLiftWheel();  //Nullt den Encouder des Motors MotorLiftWheel
int lowerWheel(int enable);       //Senkt Schaufelrad, int enable -> Senkt falls true, Stoppt bei false, rückgabewert -> false, wenn untere pos erreicht
void WheelToUpperPos();           //Hebt Schaufelrad in die obere Endlage   (Volle Geschwindigkeit)
void WheelTo10cm();               //Senkt Schaufelrad auf 10cm    (Volle Geschwindigkeit)

//container.cpp
void tiltContainer(int enable);   //Kippt Behälter, int enable -> Kippt falls true

#endif