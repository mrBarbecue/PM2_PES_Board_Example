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

    Drive Drive;

    // this loop will run forever
    while(true){
        MainTaskTimer.reset();

        if(executeMainTask){

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