#include <Arduino.h>
#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>
#include "Rover.h"

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
};

// photoresistor logic - true = high, false = low
/**
 * Checks if photoresistor is off the line
 * @param pr Photoresistor to check (int {0 = L, 1 = M, 2 = R})
 * @returns bool - Whether or not photoresistor is off line
**/
bool Rover::isOffLine(int pr) {
    if (pr==0) {
        return (analogRead(_L_PHOTORESISTOR) < 960); 
    } else if (pr==1) {
        return (analogRead(_M_PHOTORESISTOR) < 900);
    } else if (pr==2) {
        return (analogRead(_R_PHOTORESISTOR) < 900);
    } else {
        return false
    }
}

/**
*Turns rover right
*@param degrees Amount of degrees to turn right. (int 0-33)
**/
void Rover::steerRight(int deg) {
    if (deg < 0 || deg > 33) {
        deg = deg < 0 ? 0:33
    }

    _largeservo.write(92 + deg);
    Serial.print("Turning right by ");
    Serial.print(deg);
    Serial.print(" degrees\n");
};

/**
* Turns rover left
* @param degrees Amount of degrees to turn left. (int 0-33)
**/
void Rover::steerLeft(int deg) {
    if (deg < 0 || deg > 33) {
        deg = deg < 0 ? 0:33
    }
    _largeservo.write(92 - deg);
    Serial.print("Turning left by ");
    Serial.print(deg);
    Serial.print(" degrees\n");
};

/**
* Resets rover turning
**/
void Rover::steerStraight() // servo is set to 92deg for going straight, aligns better with steering system
{
    _largeservo.write(92);
    Serial.println("Steering reset");
};

/**
* Sets motor speed
* @param speed Percentage of maximum speed (int 0-100)
**/
void Rover::motorSet(int speed)
{
    speed = speed * 1.8; // since motor is servo object, 100% = 180deg
    motor0.write(speed);
    Serial.print("Speed set to ");
    Serial.print(speed);
    Serial.print("\n");
};

/**
 * Opens/closes claw
 * @param status Whether or not claw is open (bool)
**/
void Rover::clawSet(bool status) {
    if (status){
        Serial.print('Closing claw')
        _smallservo.write(0)
    } else {
        Serial.print('Opening claw')
        _smallservo.write(80)
    }
};

/**
 *  Sets claw to specific angle
 *  @param deg Claw angle (int 0-80)
**/
void Rover::clawSetPos(int deg) {
    if (deg < 0 || deg > 80) {
        deg = deg < 0 ? 0:80
    }
    _smallservo.write(deg)
    Serial.print("Setting claw to ");
    Serial.print(deg);
    Serial.print(" degrees\n");
};

/**
 * Sets LED color
 * @param group LED Light group (int {0 = Group 0, 1 = Group 1})
 * @param R LED Red value (int 0-255)
 * @param G LED Green value (int 0-255)
 * @param B LED Blue value (int 0-255)
**/
void Rover::colorSet(int group, int R, int G, int B) 
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
};

/**
 * Makes rover dance
**/
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
};

/**
 * Makes rover a gamer
**/
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
};