#include "headers.h"
#define PinEnableDcMotors PB_ENABLE_DCMOTORS

bool executeMainTask = false;
bool resetAll = false;
//Instanziert Objekt um UserButten zu entprellen
DebounceIn UserButton(USER_BUTTON);

//Instanziert Objekt um DC-Motoren anzusteuern
DigitalOut EnableMotors(PinEnableDcMotors);

void executeMainFunction(){  //Funktion wird aufgerufen, wenn der UserButton gedrückt wurde
    //Schaltet executeMainTask um, wenn der UserButton gedrückt wurde
    executeMainTask = !executeMainTask;
    //Bei positiver Flanke wird resetAll true gesetzt
    if(executeMainTask)
        resetAll = true;
}

int main(){
    //Funktion wird aufgerufen, wenn der UserButton gedrückt wird
    UserButton.fall(&executeMainFunction);

    const int mainTaskPeriod = 40;  //Minimalzeit in ms pro maindurchlauf
    Timer MainTaskTimer;            //Erzeugt MainTaskTimer Objekt;

    //Start timer-
    MainTaskTimer.start();

    const int servoTiltTime = 10000;     //Zeit (in ms) wie lange der Behälter ausgekippt werden soll
    const int loopsServoTiltTime = servoTiltTime / mainTaskPeriod; //Berechnet anzahl durchläufe in main bis servoTiltTime vergangen ist
    int counterServoTiltTime = 0;   //Zählt loops für servoTiltTime

    const float driveToTargetContainer = 4.5f;  //Nach wievielen Minuten der Roboter zum Zielbehälter fahren soll
                                                //Sorgt dafür, dass sicher nach 5min ein paar Perlen in Zielbehälter sind
    const int loopsDriveToTargetContainer = driveToTargetContainer * 60000 / mainTaskPeriod;   //Berechnet anzahl durchläufe bis Zeit von driveToTargetContainer
                                                                                                //vergangen ist
    int counterDriveToTargetContainer = 0;   //Zählt loops für driveToTargetContainer

    bool containerFull = false; //Falls der Container voll ist

    //Erstellt Objekte, da es von allen Klassen je nur ein Objekt braucht, heissen sie gleich wie ihre Klassen
    Drive Drive;
    Mining Mining;
    Container Container;

    bool liftWheelInitialized = false;      //Speichert ob der Motor zum heben des Rads bereits initialisiert wurde
    bool driveMotorsInitialized = false;    //Speichert on driveMotors bereits initialisiert wurden, wird benötigt,
                                            //dass wegen der ungenauigkeit des IR-Sensors der Roboter nach dem
                                            //anhalten nicht erneut los fährt

    //Verschiedene Zustände in switch-case                                        
    enum states{                            
    initializeLiftWheel,
    initializeDriveMotors,
    mining,
    wheelToUpperPos,
    nextPos,
    targetContainer,
    beforeNextPos,
    test};
    //Speichert die Zustände in switch-case
    int state = 0;

    printf("\n\n\n");

    while(true){
        MainTaskTimer.reset();
        counterDriveToTargetContainer++; //Zählt jeden while durchlauf
        //Wenn Zeit in Minuten von driveToTargetContainer vorbei sind, fährt der Roboter zum Zielbehälter um restliche perlen auszuladen
        if(counterDriveToTargetContainer == loopsDriveToTargetContainer && state != targetContainer){
            state = wheelToUpperPos;
            printf("%.1fmin have passed", driveToTargetContainer);
        }
        if(executeMainTask){
            EnableMotors = true;
            switch(state){
                case test:
                    Container.tiltContainer(true);
                    //Zählt loops
                    counterServoTiltTime++;
                    if(counterServoTiltTime >= loopsServoTiltTime){
                        //Fährt den Behälter wieder ein
                        counterServoTiltTime = 0; //Zähler zurücksetzten
                        Container.tiltContainer(false);
                        state = 100;
                    }
                    break;
                    
                case initializeLiftWheel:
                    //Nullt den Antrieb, der das Schaufelrad hebt
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
                    Drive.initializeDriveMotors();
                    Drive.calculatePositions();
                    state = nextPos;
                    break;

                case mining:
                    //Lässt Schaufelrad drehen
                    Mining.spinWheel(true);
                    //Falls das Schaufelrad am Boden ankommt oder der Container voll ist, wird die nächste hintere Position angefahren
                    containerFull = Container.containerFull();
                    if(Mining.lowerWheel() || containerFull){
                        state = wheelToUpperPos;
                    }
                    break;

                case wheelToUpperPos:
                    if(Mining.wheelToUpperPos()){
                        //Wenn Container voll ist, Schaufelrad in die obere Endlage, Rad verzögert ausschalten (um restliche Perlen noch in eigenen Behälter zu befördern) 
                        Mining.spinWheel(false);
                        //Fährt zuerst ein stück weg vom Behälter um Platz für die Drehung zu schaffen
                        state = nextPos;
                    }
                    break;

                case nextPos:
                    printf("nextPos\n");
                    if(Drive.lastPositionReached() || containerFull){
                        if(Drive.driveRelative(0, 80, false)){
                        containerFull = false;
                        state = targetContainer;   
                        }
                    }
                    //Weis an welcher Position der Roboter steht und wie der Roboter zu nächsten Position fahren muss, sobal er angekommen ist,
                    //fängt er wieder an Perlen aufzusammeln
                    else if(Drive.driveToNextPosition()){
                        state = mining;
                    }
                    break;

                case targetContainer:
                    printf("targetContainer\n");
                    Drive.deleteCurrentPos(); //Löscht Aufsammel-Positonen die bereits angefahren wurden
                    if(Drive.toTargetContainer()){
                        //Kippt den Behälter
                        Container.tiltContainer(true);
                        //Zählt loops
                        counterServoTiltTime++;
                        //Wenn zeit vergange ist, wird der Behälter wieder eingefahren
                        if(counterServoTiltTime >= loopsServoTiltTime){
                            //Fährt den Behälter wieder ein
                            Container.tiltContainer(false);
                            counterServoTiltTime = 0; //Zähler zurücksetzten
                            state = beforeNextPos;
                        }
                    }
                    break;

                case beforeNextPos:
                    printf("beforeNextPos\n");
                    if(Drive.driveInFrontOfPos()){
                        state = nextPos;
                    }
                    break;
            }
        }
        else{
            //wird nach drücken des UserButton einmal ausgeführt
            if(resetAll){
                /*
                resetAll = false;
                
                EnableMotors = false;
                liftWheelInitialized = false;
                driveMotorsInitialized = false;
                counterServoTiltTime = 0;
                counterDriveToTargetContainer = 0;

                state = initializeLiftWheel;
                */
            }
        }
        
        //read timer and make the main thread sleep for the remaining time span (non blocking)
        int mainTaskElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(MainTaskTimer.elapsed_time()).count();
        //printf("\nMainTaskElapsedTime: %d\n", mainTaskElapsedTime);
        thread_sleep_for(mainTaskPeriod - mainTaskElapsedTime);
    }
}