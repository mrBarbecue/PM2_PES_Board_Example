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

    const int mainTaskPeriod = 20;  //Minimalzeit in ms pro maindurchlauf
    Timer MainTaskTimer;            //Erzeugt MainTaskTimer Objekt;

    //Start timer
    MainTaskTimer.start();

    //Verschiedene Zustände in switch-case
    enum states{
    initializeLiftWheel, initializeDriveMotors, mining, wheelTo10cm, wheelToUpperPos, nextPos, targetContainer};
    //Speichert die Zustände in switch-case
    int state = 0;

    const int servoTiltTime = 3000;     //Zeit (in ms) wie lange der Behälter ausgekippt werden soll
    const int loopsServoTiltTime = servoTiltTime / mainTaskPeriod; //Berechnet anzahl durchläufe in main bis servoTiltTime vergangen ist
    int counterServoTiltTime = 0;   //Zählt loops für servoTiltTime

    const float driveToTargetContainer = 4.5f;  //Nach wievielen Minuten der Roboter zum Zielbehälter fahren soll
                                                //Sorgt dafür, dass sicher nach 5min ein paar Perlen in Zielbehälter sind
    const int loopsDriveToTargetContainer = driveToTargetContainer * 60000 / mainTaskPeriod;   //Berechnet anzahl durchläufe bis Zeit von driveToTargetContainer
                                                                                                //vergangen ist
    int counterDriveToTargetContainer = 0;   //Zählt loops für driveToTargetContainer

    //Erstellt Objekte, da es von allen Klassen je nur ein Objekt braucht, heissen sie gleich wie ihre Klassen
    Drive Drive;
    Mining Mining;
    Container Container;

    bool liftWheelInitialized = false;      //Speichert ob der Motor zum heben des Rads bereits initialisiert wurde
    bool driveMotorsInitialized = false;    //Speichert on driveMotors bereits initialisiert wurden, wird benötigt,
                                            //dass wegen der ungenauigkeit des IR-Sensors der Roboter nach dem
                                            //anhalten nicht erneut los fährt

    while(true){
        MainTaskTimer.reset();

        counterDriveToTargetContainer++; //Zählt jeden while durchlauf
        //Wenn Zeit in Minuten von driveToTargetContainer vorbei sind, fährt der Roboter zum Zielbehälter um restliche perlen auszuladen
        if(counterDriveToTargetContainer == loopsDriveToTargetContainer && state != wheelToUpperPos && state != targetContainer){
            state = wheelToUpperPos;
        }

        if(executeMainTask){

            switch(state){
                case initializeLiftWheel:
                    //Nullt den Antrieb, der das Schaufelrad hebt
                    EnableMotors = true;
                    if(!liftWheelInitialized){
                        liftWheelInitialized = Mining.initializeMotorLiftWheel();
                    }
                    else{
                        //Sobal Schaufelrad in der oberen Endlange ist, wird dieser case verlassen
                        if(Mining.WheelToUpperPos()){
                            state = initializeDriveMotors;
                        }
                    }
                    break;

                case initializeDriveMotors:
                    //Fährt auf den Startcontainer zu un stoppt bei der ersten Position um Perlen aufsammeln zu können
                    if(Drive.initializeDriveMotors()){
                        if(!driveMotorsInitialized){
                            driveMotorsInitialized = Mining.WheelTo10cm();
                        }
                        else{
                            //Berechnet Koordinaten der Positionen wo Perlen aufgesammelt werden
                            Drive.calculatePositions();
                            state = mining;
                        }
                    }
                    break;

                case mining:
                    //Lässt Schaufelrad drehen
                    Mining.spinWheel(true);
                    //Falls das Schaufelrad am Boden ankommt, wird das Schaufelrad ausgeschaltet und eine andere Aufsammelposition angefahren
                    if(Mining.lowerWheel()){
                        state = wheelTo10cm;
                    }
                    //Während Perlen aufgeladen werden, wird gecheckt ob der Container voll ist
                    if(Container.containerFull()){
                        //Wenn eigener Behälter voll ist
                        state = wheelToUpperPos;
                    }
                    break;

                case wheelTo10cm:    
                    //Sobal Schaufelrad überhalb der Perlen ist, wird neue Position angefahren
                    if(Mining.WheelTo10cm()){
                        state = nextPos;
                    }
                    break;

                case wheelToUpperPos:
                    if(Mining.WheelToUpperPos()){
                        //Wenn Container voll ist, Schaufelrad in die obere Endlage, Rad verzögert ausschalten (um restliche Perlen noch in eigenen Behälter zu befördern) 
                        Mining.spinWheel(false);
                        state = targetContainer;
                    }
                    break;

                case nextPos:
                    //Weis an welcher Position der Roboter steht und wie der Roboter zu nächsten Position fahren muss, sobal er angekommen ist,
                    //fängt er wieder an Perlen aufzusammeln
                    if(Drive.driveToNextPosition()){
                        state = mining;
                    }
                    break;

                case targetContainer:
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
                            state = nextPos;
                            counterServoTiltTime = 0; //Zähler zurücksetzten
                        }
                    }
                    break;
            }
        }
        else{
            //wird nach drücken des UserButton einmal ausgeführt
            if(resetAll){
                resetAll = false;
                
                EnableMotors = false;
                liftWheelInitialized = false;
                driveMotorsInitialized = false;
                counterServoTiltTime = 0;
                counterDriveToTargetContainer = 0;

                state = initializeLiftWheel;
            }
        }
        //read timer and make the main thread sleep for the remaining time span (non blocking)
        int MainTaskElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(MainTaskTimer.elapsed_time()).count();
        thread_sleep_for(mainTaskPeriod - MainTaskElapsedTime);
    }
}