/*
    ENGR 7B Autonomous Rover Control Library
*/

#ifndef Rover_h
#define Rover_h

#include <Arduino.h>
#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>

/**
 * Rover library
 * @param onLine()
 * @param turnRight()
 * @param turnLeft()
 * @param turnStraight()
 * @param motorSpeed()
 * @param clawSet()
 * @param clawSetPos()
 * @param setColor()
 * @param dance()
 * @param rainbowLights()
**/
class Rover
{
    public:
        Rover(
            int STEERING_SERVO,
            int CLAW_SERVO,
            int MOTOR,
            uint8_t L_PHOTORESISTOR,
            uint8_t R_PHOTORESISTOR,
            uint8_t M_PHOTORESISTOR,
            int LED0_R,
            int LED0_G,
            int LED0_B,
            int LED1_R,
            int LED1_G,
            int LED1_B
        );

        void begin();
        void calibrate(unsigned long int min[3], unsigned long int max[3]);

        //steering controls
        void steerLeft(int degrees);
        void steerRight(int degrees);
        void steerStraight();

        //claw controls
        void clawSet(bool status);
        void clawSetPos(int degrees);

        //motor controls
        void motorSet(int speed);

        //photoresistor logic

        int isOffLine(int pr);
        //led controls
        void colorSet(int group, int R, int G, int B);
        void colorFlash(int group, int R, int G, int B, int delay);

        //fun functions
        void dance();
        void rainbowLights();

        //Pixy object
        Pixy2 pixy;
    
    private:
        unsigned long int _min[3];
        unsigned long int _max[3];
        int _STEERING_SERVO;
        int _CLAW_SERVO;
        int _MOTOR;

        uint8_t _L_PHOTORESISTOR;
        uint8_t _R_PHOTORESISTOR;
        uint8_t _M_PHOTORESISTOR;

        int _LED0_R;
        int _LED0_G;
        int _LED0_B;

        int _LED1_R;
        int _LED1_G;
        int _LED1_B;

        Servo _largeservo;
        Servo _smallservo;
        Servo _motor;
};

#endif
