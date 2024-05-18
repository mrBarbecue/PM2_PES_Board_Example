#include "headers.h"

//Pin um DC-Motoren zu aktivieren
#define PinEnableDcMotors PB_ENABLE_DCMOTORS

bool executeMainTask = false;   //Speichert ob der main durchlaufen werden soll
bool resetAll = false;          //Flanke um Variablen rückzusetzten

//Instanziert Objekt um UserButten zu entprellen
DebounceIn UserButton(USER_BUTTON);

//Instanziert Objekt um DC-Motoren anzusteuern
DigitalOut EnableMotors(PinEnableDcMotors);

//Funktion wird aufgerufen, wenn der UserButton gedrückt wurde
void executeMainFunction(){  
    //Schaltet executeMainTask um, wenn der UserButton gedrückt wurde
    executeMainTask = !executeMainTask;
    //Bei positiver Flanke wird resetAll true gesetzt
    if(executeMainTask)
        resetAll = true;
}

int main(){
    //Funktion wird aufgerufen, wenn der UserButton gedrückt wird
    UserButton.fall(&executeMainFunction);

    const int mainTaskPeriod = 20;  //Minimalzeit in ms pro maindurchlauf
    Timer MainTaskTimer;            //Erzeugt MainTaskTimer Objekt;

    //Startet den Timer
    MainTaskTimer.start();

    const int servoTiltTime = 5000;     //Zeit (in ms) wie lange der Behälter ausgekippt werden soll
    const int loopsServoTiltTime = servoTiltTime / mainTaskPeriod; //Berechnet anzahl durchläufe in main bis servoTiltTime vergangen ist
    int counterServoTiltTime = 0;       //Zählt loops für servoTiltTime

    const float driveToTargetContainer = 4.5f;  //Nach wievielen Minuten der Roboter zum Zielbehälter fahren soll
                                                //Sorgt dafür, dass sicher nach 5min ein paar Perlen in Zielbehälter sind
    const int loopsDriveToTargetContainer = driveToTargetContainer * 60000 / mainTaskPeriod;   //Berechnet anzahl durchläufe bis Zeit von driveToTargetContainer
                                                                                                //vergangen ist
    int counterDriveToTargetContainer = 0;   //Zählt loops für driveToTargetContainer

    bool containerFull = false; //Speichert ob der Container voll ist um zum Zielbehälter zu fahren

    //Erstellt Objekte, da es von allen Klassen je nur ein Objekt braucht, heissen sie gleich wie ihre Klassen
    Drive Drive;
    Mining Mining;
    Container Container;

    bool liftWheelInitialized = false;  //Speichert ob der Motor zum heben des Rads bereits initialisiert wurde

    //Verschiedene Zustände in switch-case                                        
    enum states{                            
    initializeLiftWheel,
    initializeDriveMotors,
    wheelTo10cm,
    mining,
    wheelToUpperPos,
    nextPos,
    targetContainer,
    beforeNextPos,
    test};
    //Speichert die Zustände des switch-case
    int state = 0;

    while(true){
        MainTaskTimer.reset();  //resettet jeden durchlauf den Timer
        counterDriveToTargetContainer++; //Zählt jeden while durchlauf
        //Wenn Zeit in Minuten von driveToTargetContainer vorbei sind, fährt der Roboter zum Zielbehälter um restliche perlen auszuladen
        if(counterDriveToTargetContainer == loopsDriveToTargetContainer && state != targetContainer){
            state = wheelToUpperPos;
            //printf("%.1fmin have passed", driveToTargetContainer);
        }
        if(executeMainTask){
            EnableMotors = true;

            switch(state){
                case test:
                    //Für Tests
                    break;
                    
                case initializeLiftWheel:
                    //Initialisiert den Antrieb, der das Schaufelrad hebt
                    if(!liftWheelInitialized){
                        liftWheelInitialized = Mining.initializeMotorLiftWheel();
                    }
                    else{
                        //Sobal Schaufelrad in der oberen Endlange ist, wird dieser case verlassen
                        if(Mining.wheelToUpperPos()){
                            state = initializeDriveMotors;
                        }
                    }
                    break;

                case initializeDriveMotors:
                    //Initialisiert die Motoren zum fahren (über den IR-Sensor)
                    //und setzt die Startposition
                    Drive.initializeDriveMotors();
                    //Berechnet die Aufsammelstellen
                    Drive.calculatePositions();
                    state = nextPos;
                    break;

                case mining:
                    //Lässt Schaufelrad drehen
                    Mining.spinWheel(true);
                    //Falls das Schaufelrad am Boden ankommt oder der Container voll ist, geht es zum nächsten case
                    containerFull = Container.containerFull();
                    if(Mining.lowerWheel() || containerFull){
                        state = wheelToUpperPos;
                    }
                    break;

                case wheelToUpperPos:
                    //Fährt Förderband in die Obere Endlage
                    if(Mining.wheelToUpperPos()){
                        //Schaltet das Schaufelrad aus
                        Mining.spinWheel(false);
                        state = nextPos;
                    }
                    break;

                case nextPos:
                    if(Drive.lastPositionReached() || containerFull){
                        //Fährt von der aktuellen Stelle 80mm rückwärts (positive Y-Richtung)
                        if(Drive.driveRelative(0, 80, false)){
                        //Setzt Varible wieder zurück
                        containerFull = false;
                        state = targetContainer;   
                        }
                    }
                    //Weis an welcher Position der Roboter steht und wie der Roboter zu nächsten Position fahren muss
                    else if(Drive.driveToNextPosition()){
                        state = wheelTo10cm;
                    }
                    break;

                case wheelTo10cm:
                    //Senkt Schaufelrad mit voller Geschwindigkeit auf 10cm 
                    if(Mining.wheelTo10cm()){
                        state = mining;
                    }

                case targetContainer:
                    //Löscht Aufsammel-Positonen die bereits angefahren wurden
                    Drive.deleteCurrentPos();
                    //Fährt zum Zielbehälter
                    if(Drive.toTargetContainer()){
                        //Kippt den Behälter
                        Container.tiltContainer(true);
                        //Zählt loops
                        counterServoTiltTime++;
                        //Wenn zeit vergange ist, wird der Behälter wieder eingefahren
                        if(counterServoTiltTime >= loopsServoTiltTime){
                            //Fährt den Behälter wieder ein
                            Container.tiltContainer(false);
                            //Zähler zurücksetzten
                            counterServoTiltTime = 0;
                            state = beforeNextPos;
                        }
                    }
                    break;

                case beforeNextPos:
                    //Fährt vom Zielbehälter vor die nächste Position,
                    //versichert, dass der Roboter beim drehen nicht an den Startbehälter ankommt
                    if(Drive.driveInFrontOfPos()){
                        state = nextPos;
                    }
                    break;
            }
        }
        else{
            //wird nach drücken des UserButton einmal ausgeführt
            if(resetAll){
                
            }
        }
        
        //read timer and make the main thread sleep for the remaining time span (non blocking)
        int mainTaskElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(MainTaskTimer.elapsed_time()).count();
        //printf("\nMainTaskElapsedTime: %d\n", mainTaskElapsedTime);
        thread_sleep_for(mainTaskPeriod - mainTaskElapsedTime);
    }
}