/*
    ENGR 7B Autonomous Rover Control Library
*/

#ifndef Rover_h
#define Rover_h

#include <Arduino.h>
#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>

class Rover
{
    public:
        Rover(
            int STEERING_SERVO,
            int CLAW_SERVO,
            int MOTOR,
            String L_PHOTORESISTOR,
            String R_PHOTORESISTOR,
            String M_PHOTORESISTOR,
            int LED0_R,
            int LED0_G,
            int LED0_B,
            int LED1_R,
            int LED1_G,
            int LED1_B
        );

        //steering controls
        void turnLeft(int degrees);
        void turnRight(int degrees);
        void turnStraight();

        //claw controls
        void clawOpen(int degrees);
        void clawClose(int degrees);

        //motor controls
        void motorSpeed(int speed);

        //photoresistor logic
        // photoresistor logic - true = high, false = low
        bool leftPR();
        bool midPR();
        bool rightPR();

        //led controls
        void setColor(int group, int R, int G, int B);

        //fun functions
        void dance();
        void rainbowLights();
    
    private:
        int _STEERING_SERVO;
        int _CLAW_SERVO;
        int _MOTOR;

        int _L_PHOTORESISTOR;
        int _R_PHOTORESISTOR;
        int _M_PHOTORESISTOR;

        int _LED0_R;
        int _LED0_G;
        int _LED0_B;

        int _LED1_R;
        int _LED1_G;
        int _LED1_B;

        Pixy2 _pixy;
        Servo _largeservo;
        Servo _smallservo;
        Servo _motor;
};

#endif