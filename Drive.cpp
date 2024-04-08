#include "headers.h"

#define PinMotorDriveRight PB_PWM_M1
#define PinMotorDriveLeft PB_PWM_M2
#define PinEnableDcMotors PB_ENABLE_DCMOTORS

#define PinEncoderMotorRightA PB_ENC_A_M1
#define PinEncoderMotorRightB PB_ENC_B_M1
#define PinEncoderMotorLeftA PB_ENC_A_M2
#define PinEncoderMotorLeftB PB_ENC_B_M2

/*
PWM input 0.0f: -12V
PWM input 0.5f: 0V
PWM input 1.0f: 12V
*/

//motorDriveRight.setRotation(3.0f);
//motorDriveLeft.setRotation(3.0f);

class Drive{

    private:
        //motorDriveRight und motorDriveRight Getriebe
        const float gearRatioMotorDrive = 100.0f;   // gear ratio
        const float rpmV = 140.0f / 12.0f;          // motor constant [rpm/V]
        const float voltageMax = 12.0f;
        const float maxVelocity = 5.14f / voltageMax;

    public:
        Drive(){                          //Konsturktor

            // create object to enable power electronics for the DC motors
            DigitalOut enableMotors(PinEnableDcMotors);
            // enable hardwaredriver DC motors: 0 -> disabled, 1 -> enabled
            enableMotors = 1; // setting this once would actually be enough


            DCMotor motorDriveRight(PinMotorDriveRight, PinEncoderMotorRightA, PinEncoderMotorRightB, gearRatioMotorDrive, rpmV, voltageMax);
            // enable the motion planner for smooth movement
            motorDriveRight.enableMotionPlanner(true);
            // limitiert Maximumgeschwindigkeit
            motorDriveRight.setMaxVelocity(motorDriveRight.getMaxPhysicalVelocity() * maxVelocity);


            DCMotor motorDriveLeft(PinMotorDriveLeft, PinEncoderMotorLeftA, PinEncoderMotorLeftB, gearRatioMotorDrive, rpmV, voltageMax);
            // enable the motion planner for smooth movement
            motorDriveLeft.enableMotionPlanner(true);
            // limitiert Maximumgeschwindigkeit
            motorDriveLeft.setMaxVelocity(motorDriveLeft.getMaxPhysicalVelocity() * maxVelocity);

        }

        void nextPosition(int pos){       //Fährt zur nächsten Position vor dem Startbehälter, int pos -> welche position vor dem Behälter

        /*
        Update the printing command to print the number of rotations:
        printf("Motor position: %f \n", motor_M3.getRotation());
        Then include the command that will rotate the motor 3 times:
        motor_M3.setRotation(3.0f);
        */
        }
        void toStartContainer(){          //Fährt zu Startbehälter

        }
        void toFinishContainer(){         //Fährt zum Zielbehälter

        }

    private:


};