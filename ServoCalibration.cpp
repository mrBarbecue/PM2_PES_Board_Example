#if false

#include "headers.h"

#define PinServoTilt PB_D2

Servo servoTilt(PinServoTilt);

float servo_input = 0.0f;
int servo_counter = 0; // define servo counter, this is an additional variable
                       // used to command the servo
const int loops_per_seconds = static_cast<int>(ceilf(1.0f / (0.001f * static_cast<float>(main_task_period_ms))));

// print to the serial terminal
printf("Pulse width: %f \n", servo_input);

// enable the servos
if (!servoTilt.isEnabled())
    servoTilt.enable();


// command the servos
servoTilt.setNormalisedPulseWidth(servo_input);

// calculate inputs for the servos for the next cycle
if ((servo_input < 1.0f) &&                     // constrain servo_input to be < 1.0f
    (servo_counter % loops_per_seconds == 0) && // true if servo_counter is a multiple of loops_per_second
    (servo_counter != 0))                       // avoid servo_counter = 0
    servo_input += 0.005f;
servo_counter++;

// reset variables and objects
servo_D0.disable();
servo_input = 0.0f;

// minimal pulse width and maximal pulse width obtained from the servo calibration process
// futuba S3001
float servo_D0_ang_min = 0.0150f; // carefull, these values might differ from servo to servo
float servo_D0_ang_max = 0.1150f;
// reely S0090
float servo_D1_ang_min = 0.0325f;
float servo_D1_ang_max = 0.1175f;

// servo.setNormalisedPulseWidth: before calibration (0,1) -> (min pwm, max pwm)
// servo.setNormalisedPulseWidth: after calibration (0,1) -> (servo_D0_ang_min, servo_D0_ang_max)
servoTilt.calibratePulseMinMax(servo_D0_ang_min, servo_D0_ang_max);

#endif