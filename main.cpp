#include "headers.h"
#define PinEnableDcMotors PB_ENABLE_DCMOTORS

bool executeMainTask = false; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool resetAll = false;    // this variable is used to reset certain variables and objects and
                                   // shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(USER_BUTTON); // create DebounceIn object to evaluate the user button
                                     // falling and rising edge

// create object to enable power electronics for the DC motors
DigitalOut enableMotors(PinEnableDcMotors);


void executeMainFunction(){  // custom function which is getting executed when userbutton gets pressed

    // toggle executeMainTask if the button was pressed
    executeMainTask = !executeMainTask;
    // set resetAll to true if executeMainTask changed from false to true
    if (executeMainTask)
        resetAll = true;
}

// main runs as an own thread
int main()
{
    // attach button fall function address to user button object, button has a pull-up resistor
    user_button.fall(&executeMainFunction);

    // while loop gets executed every mainTaskPeriod milliseconds, this is a
    // simple approach to repeatedly execute main
    const int mainTaskPeriod = 200; // define main task period time in ms e.g. 20 ms, there for
                                        // the main task will run 50 times per second
    Timer MainTaskTimer;              // create Timer object which we use to run the main task
                                        // every mainTaskPeriod

    // led on nucleo board
    DigitalOut user_led(USER_LED);

    // start timer
    MainTaskTimer.start();

    // enable hardwaredriver DC motors: 0 -> disabled, 1 -> enabled
    enableMotors = 1; // setting this once would actually be enough

    Mining Wheel;

    // this loop will run forever
    while (true) {
        MainTaskTimer.reset();
        Wheel.initializeMotorLiftWheel();

        if (executeMainTask) {
            Wheel.WheelTo10cm();

        }
        else{
            
            // the following code block gets executed only once
            if (resetAll) {
                resetAll = false;


            }
        }

        // toggling the user led
        user_led = !user_led;

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int MainTaskElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(MainTaskTimer.elapsed_time()).count();
        thread_sleep_for(mainTaskPeriod - MainTaskElapsedTime);
    }
}