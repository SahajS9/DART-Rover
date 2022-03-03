#include <Arduino.h>
#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>
#include "Rover.h"

Rover::Rover(
    int STEERING_SERVO,
    int CLAW_SERVO,
    int MOTOR,
    int L_PHOTORESISTOR,
    int R_PHOTORESISTOR,
    int M_PHOTORESISTOR,
    int LED0_R,
    int LED0_G,
    int LED0_B,
    int LED1_R,
    int LED1_G,
    int LED1_B
) {
    _STEERING_SERVO = STEERING_SERVO;
    _CLAW_SERVO = CLAW_SERVO;
    _MOTOR = MOTOR;
    _L_PHOTORESISTOR = L_PHOTORESISTOR;
    _R_PHOTORESISTOR = R_PHOTORESISTOR;
    _M_PHOTORESISTOR = M_PHOTORESISTOR;
    _LED0_R = LED0_R;
    _LED0_G = LED0_G;
    _LED0_B = LED0_B;
    _LED1_R = LED1_R;
    _LED1_G = LED1_G;
    _LED1_B = LED1_B;

    Pixy2 _pixy;
    Servo _largeservo;
    Servo _smallservo;
    Servo _motor;

    // pin attachments
    _largeservo.attach(_STEERING_SERVO);
    _smallservo.attach(_CLAW_SERVO);
    _motor.attach(_MOTOR);

    // pixy cam
    _pixy.init();
    _pixy.changeProg("ccc");
    // leds - red blue green, left (cathode) middle right
    // function uses standard RGB format
    pinMode(_LED0_R, OUTPUT);
    pinMode(_LED0_G, OUTPUT);
    pinMode(_LED0_B, OUTPUT);
    pinMode(_LED1_R, OUTPUT);
    pinMode(_LED1_G, OUTPUT);
    pinMode(_LED1_B, OUTPUT);
}

// photoresistor logic - true = high, false = low
bool Rover::() { return (analogRead(A0) < 960); }
bool Rover::midPR() { return (analogRead(A1) < 900); }
bool Rover::rightPR() { return (analogRead(A2) < 940); }

void Rover::turnRight(int deg) {
    _largeservo.write(92 + deg);
    Serial.print("Turning right by ");
    Serial.print(deg);
    Serial.print(" degrees\n");
}

void Rover::turnLeft(int deg) {
    _largeservo.write(92 - deg);
    Serial.print("Turning left by ");
    Serial.print(deg);
    Serial.print(" degrees\n");
}

void Rover::turnStraight() // servo is set to 92deg for going straight, aligns better with steering system
{
    _largeservo.write(92);
    Serial.println("Steering reset");
}

void Rover::motorSpeed(int speed)
{
    speed = speed * 1.8; // since motor is servo object, 100% = 180deg
    motor0.write(speed);
    Serial.print("Speed set to ");
    Serial.print(speed);
    Serial.print("\n");
}

void Rover::setColor(int group, int R, int G, int B) 
{
    if (group == 0) {
        analogWrite(_LED0_R, R);
        analogWrite(_LED0_G, G);
        analogWrite(_LED0_B, B);
        Serial.println("Changed indicator color");
    } else if (group == 1) {
        analogWrite(_LED1_R, R);
        analogWrite(_LED1_G, G);
        analogWrite(_LED1_B, B);
        Serial.println("Changed underglow color");
    }
}

void Rover::dance() 
{
    int x = 30; // will run for ~30 seconds
    rainbowLights();
    while (x > 0)
    {
        turnLeft(45);
        delay(250);
        turnRight(45);
        delay(250);
        turnLeft(45);
        delay(250);
        turnRight(45);
        delay(250);
        x--;
    }
}

void Rover::rainbowLights()
{
    int rgbColor[3] = {255, 0, 0};
    for (int decColor = 0; decColor < 3; decColor += 1)
    {
        int incColor = decColor == 2 ? 0 : decColor + 1;
        // cross-fade the two colours.
        for (int x = 0; x < 255; x += 1)
        {
            rgbColor[decColor] -= 1;
            rgbColor[incColor] += 1;

            Rover::setColor(0, rgbColor[0], rgbColor[1], rgbColor[2]);
            Rover::setColor(1, rgbColor[0], rgbColor[1], rgbColor[2]);
            delay(5);
        }
    }
}