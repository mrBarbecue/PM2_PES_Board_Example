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

    const int mainTaskPeriod = 60;  //Minimalzeit in ms pro maindurchlauf
    Timer MainTaskTimer;            //Erzeugt MainTaskTimer Objekt;

    //Start timer
    MainTaskTimer.start();

    //Verschiedene Zustände in switch-case
    enum states{
    initializeDriveMotors, mining, wheelTo10cm, wheelToUpperPos, nextPos, targetContainer};
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

    float maxIrValue = 0.728f;
    float irBuffer = 0;

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
        if(executeMainTask){
            EnableMotors = true;
            irBuffer = Drive.getIrSensor();
            if(maxIrValue < irBuffer){
                maxIrValue = irBuffer;
                printf("maxIrValue: %.3f\n\n", maxIrValue);
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


            }
        }
        
        //read timer and make the main thread sleep for the remaining time span (non blocking)
        int MainTaskElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(MainTaskTimer.elapsed_time()).count();
        thread_sleep_for(mainTaskPeriod - MainTaskElapsedTime);
    }
}
