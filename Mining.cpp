#include "headers.h"

#define PinMotorLiftWheel PB_PWM_M3
#define PinEncoderLiftWheelA APB_ENC_A_M3
#define PinEncoderLiftWheelB PB_ENC_B_M3

class Mining{
    
    public:
        Mining(){

        }
        void spinnWheel(int enable){      //Dreht Schaufelrad, int enable -> Dreht falls true, Stoppt falls false

        }
        void initializeMotorLiftWheel(){  //Nullt den Encouder des Motors MotorLiftWheel

        }
        int lowerWheel(int enable){       //Senkt Schaufelrad, int enable -> Senkt falls true, Stoppt bei false, rückgabewert -> false, wenn untere pos erreicht

        return 9999999999999999999;
        }
        void WheelToUpperPos(){           //Hebt Schaufelrad in die obere Endlage   (Volle Geschwindigkeit)

        }
        void WheelTo10cm(){                //Senkt Schaufelrad auf 10cm    (Volle Geschwindigkeit)

        }

    private:


};