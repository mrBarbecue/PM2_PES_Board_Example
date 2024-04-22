#include "headers.h"
#define PinEnableDcMotors PB_ENABLE_DCMOTORS

bool executeMainTask = false;
bool resetAll = false;
//Instanziert Objekt um UserButten zu entprellen
DebounceIn UserButton(USER_BUTTON);

//Instanziert Objekt um DC-Motoren anzusteuern
DigitalOut EnableMotors(PinEnableDcMotors);


void executeMainFunction(){  // custom function which is getting executed when userbutton gets pressed

    // toggle executeMainTask if the button was pressed
    executeMainTask = !executeMainTask;
    // set resetAll to true if executeMainTask changed from false to true
    if(executeMainTask)
        resetAll = true;
}

// main runs as an own thread
int main(){
    // attach button fall function address to user button object, button has a pull-up resistor
    UserButton.fall(&executeMainFunction);

    // while loop gets executed every mainTaskPeriod milliseconds, this is a
    // simple approach to repeatedly execute main
    const int mainTaskPeriod = 20;  //Minimalzeit in ms pro maindurchlauf
    Timer MainTaskTimer;
    //DigitalOut user_led(USER_LED);

    // start timer
    MainTaskTimer.start();

    // enable hardwaredriver DC motors: 0 -> disabled, 1 -> enabled
    EnableMotors = 1; // setting this once would actually be enough

    enum states{initialize, mining, nextPos, targetContainer};
    int state = 0;

    const int servoTiltTime = 3000;  //Zeit (in ms) wie lange der Behälter ausgekippt werden soll
    const int delayStopWheel = 2000; //Zeit (in ms) wie lange Schaufelrad noch drehen soll um restliche perlen
                                     //in Behälter zu befördern
    const int loopsDelayStopWheel = delayStopWheel / mainTaskPeriod; //Berechnet anzahl loops

    Drive Drive;
    Mining Mining;
    Container Container;


    // this loop will run forever
    while(true){
        MainTaskTimer.reset();

        if(executeMainTask){

            switch(state){
                case initialize:
                    //Initialisierungen
                    //Wird eimal aufgerufen und bleibt in Methode bis Aktion ausgeführt
                    Mining.initializeMotorLiftWheel();
                    //Sobal Schaufelrad in der oberen Endlange ist, wird Schleife verlassen
                    while(Mining.WheelToUpperPos() != 0);
                    //Wird eimal aufgerufen und bleibt in Methode bis Aktion ausgeführt
                    Drive.initializeDriveMotors();
                    state = mining;
                    break;
                case mining:
                    //Sobal das Schaufelrad auf Perlenhöhe abgesenkt wurde, wird die Schleife verlassen
                    while(Mining.WheelTo10cm() != 0);
                    //Lässt Schaufelrad drehen
                    Mining.spinWheel(true);
                    //Während Perlen aufgeladen werden wird gecheckt ob der Container voll ist
                    //oder das Schaufelrad bereits in der unteren Endlage ist
                    while(Mining.lowerWheel() != 0){
                        if(Container.containerFull()){
                            state = targetContainer;
                        }
                    }
                    Mining.spinWheel(false);
                    //Sobal Schaufelrad in der oberen Endlange ist, wird Schleife verlassen
                    while(Mining.WheelToUpperPos() != 0);
                    state = nextPos;
                    break;
                case nextPos:
                    while(Drive.driveToNextPosition() != 0);
                    state = mining;
                    break;
                case targetContainer:
                    int i = 0;
                    while(Drive.toTargetContainer() != 0){
                        ++i;
                        if(i >= loopsDelayStopWheel)
                            Mining.spinWheel(false);
                    }
                    Container.tiltContainer(true);
                    thread_sleep_for(servoTiltTime);
                    Container.tiltContainer(false);
                    state = nextPos;
                    break;
            }
        }
        else{
    
            // the following code block gets executed only once
            if(resetAll){
                resetAll = false;


            }
        }

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int MainTaskElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(MainTaskTimer.elapsed_time()).count();
        thread_sleep_for(mainTaskPeriod - MainTaskElapsedTime);
    }
}