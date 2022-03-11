#include <Arduino.h>
#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>
#include "Rover.h"

// steering constant as calibrated to be cardinal and centered
const int steeringAlignment = 92;

#pragma region Init
/**
 * Rover library
 * @param onLine()
 * @param steerRight()
 * @param steerLeft()
 * @param turnStraight()
 * @param motorSpeed()
 * @param clawSet()
 * @param clawSetPos()
 * @param setColor()
 * @param flashColor()
 * @param dance()
 * @param rainbowLights()
 **/
Rover::Rover(
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
    int LED1_B)
{
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

    Pixy2 pixy;
    Servo _largeservo;
    Servo _smallservo;
    Servo _motor;
};

void Rover::begin()
{
    // pin attachments
    _largeservo.attach(_STEERING_SERVO);
    _smallservo.attach(_CLAW_SERVO);
    _motor.attach(_MOTOR);

    // pixy cam
    pixy.init();
    pixy.changeProg("ccc");
    // leds - red blue green, left (cathode) middle right
    // function uses standard RGB format
    pinMode(_LED0_R, OUTPUT);
    pinMode(_LED0_G, OUTPUT);
    pinMode(_LED0_B, OUTPUT);
    pinMode(_LED1_R, OUTPUT);
    pinMode(_LED1_G, OUTPUT);
    pinMode(_LED1_B, OUTPUT);
};

void Rover::calibrate(int min[3], int max[3]){
    _min[3] = (float)min[3];
    _max[3] = (float)max[3];
};
#pragma endregion

#pragma region Photoresistors
// photoresistor logic - true = high, false = low
/**
 * Checks if photoresistor is off the line
 * @param pr Photoresistor to check (int {0 = L, 1 = M, 2 = R})
 * @returns bool - Whether or not photoresistor has high or low signal
 **/
bool Rover::isOffLine(int pr)
{
    static int val[3] = {};
    float floatval[3] = {};

    if (pr == 0)
    {
        val[0] = 0;
        int L_raw = analogRead(_L_PHOTORESISTOR);
        val[0] = ((val[0] * 3) + L_raw) / 4;
        floatval[0] = val[0]*(1/(_max[0] - _min[0])) - _min[0]*(1/(_max[0] - _min[0]));
        
        return (floatval[0] < 300);
    }
    else if (pr == 1)
    {
        val[1] = 0;
        int M_raw = analogRead(_M_PHOTORESISTOR);
        val[1] = ((val[1] * 3) + M_raw) / 4;
        floatval[1] = val[1]*(1/(_max[1] - _min[1])) - _min[1]*(1/(_max[1] - _min[1]));
        
        return (floatval[1] < 300);
    }
    else if (pr == 2)
    {
        val[2] = 0;
        int R_raw = analogRead(_R_PHOTORESISTOR);
        val[2] = ((val[2] * 3) + R_raw) / 4;
        floatval[2] = val[2]*(1/(_max[2] - _min[2])) - _min[2]*(1/(_max[2] - _min[2]));
        
        return (floatval[2] < 300);
    }
    else
    {
        return false;
    }

    // SHARPIE LINE = L560 M370 R420
    // PRINTER PAPER = L300 M200 R270
    // TRACK = L500 M400 R350
    // BRANDYWINE @ 6:30 = 700 300 550
}
#pragma endregion

#pragma region Steering
/**
 *Turns rover right
 *@param degrees Amount of degrees to turn right. (int 0-33)
 **/
void Rover::steerRight(int deg)
{
    if (deg < 0 || deg > 50)
    {
        deg = deg < 0 ? 0 : 50;
    }

    _largeservo.write(steeringAlignment + deg);
    Serial.print("Turning right by ");
    Serial.print(deg);
    Serial.print(" degrees\n");
};

/**
 * Turns rover left
 * @param degrees Amount of degrees to turn left. (int 0-33)
 **/
void Rover::steerLeft(int deg)
{
    if (deg < 0 || deg > 50)
    {
        deg = deg < 0 ? 0 : 50;
    }
    _largeservo.write(steeringAlignment - deg);
    Serial.print("Turning left by ");
    Serial.print(deg);
    Serial.print(" degrees\n");
};

/**
 * Resets rover turning
 **/
void Rover::steerStraight() // servo is set to steeringAlignment for going straight, aligns better with steering system
{
    _largeservo.write(steeringAlignment);
    Serial.println("Steering reset");
};
#pragma endregion

#pragma region Motor
/**
 * Sets motor speed
 * @param speed Percentage of maximum speed (int -100-100)
 **/
void Rover::motorSet(int speed)
{
    speed = (speed * 0.9) + 90;
    _motor.write(speed);
    Serial.print("Speed set to ");
    Serial.print(speed);
    Serial.print("\n");
};
#pragma endregion

#pragma region Claw
/**
 * Opens/closes claw | 0deg/80deg
 * @param status True - open; false = close
 **/
void Rover::clawSet(bool status)
{
    int pos; // temp var for storing position through loop
    if (status)
    {
        Serial.print("Opening claw");
        for (pos = 80; pos >= 0; pos -= 1)
        {
            _smallservo.write(pos);
            delay(30);
        }
    }
    else
    {
        Serial.print("Closing claw");
        for (pos = 0; pos <= 50; pos += 1)
        {
            _smallservo.write(pos);
            delay(30);
        }
    }
};

/**
 *  Sets claw to specific angle
 *  @param deg Claw angle (int 0-80)
 **/
void Rover::clawSetPos(int deg)
{
    if (deg < 0 || deg > 80)
    {
        deg = deg < 0 ? 0 : 80;
    }
    _smallservo.write(deg);
    Serial.print("Setting claw to ");
    Serial.print(deg);
    Serial.print(" degrees\n");
};
#pragma endregion

#pragma region LEDs
/**
 * Sets LED color
 * @param group LED Light group (int {0 = Group 0 (indicators), 1 = Group 1 (underglow)})
 * @param R LED Red value (int 0-255)
 * @param G LED Green value (int 0-255)
 * @param B LED Blue value (int 0-255)
 **/
void Rover::colorSet(int group, int R, int G, int B)
{
    if (group == 0)
    {
        analogWrite(_LED0_R, R);
        analogWrite(_LED0_G, G);
        analogWrite(_LED0_B, B);
        Serial.println("Changed indicator color");
    }
    else if (group == 1)
    {
        analogWrite(_LED1_R, R);
        analogWrite(_LED1_G, G);
        analogWrite(_LED1_B, B);
        Serial.println("Changed underglow color");
    }
};

/**
 * @param group LED Light group (int {0 = Group 0, 1 = Group 1})
 * @param R LED Red value (int 0-255)
 * @param G LED Green value (int 0-255)
 * @param B LED Blue value (int 0-255)
 * @param delay delay between flashes (int 0-99999)
 */
void Rover::colorFlash(int group, int R, int G, int B, int del)
{
    if (group == 0)
    {
        Serial.println("Flashing indicators");
        analogWrite(_LED0_R, R);
        analogWrite(_LED0_G, G);
        analogWrite(_LED0_B, B);
        delay(del);
        analogWrite(_LED0_R, 0);
        analogWrite(_LED0_G, 0);
        analogWrite(_LED0_B, 0);
    }
    else if (group == 1)
    {
        Serial.println("Flashing underglow");
        analogWrite(_LED0_R, R);
        analogWrite(_LED0_G, G);
        analogWrite(_LED0_B, B);
        delay(del);
        analogWrite(_LED0_R, 0);
        analogWrite(_LED0_G, 0);
        analogWrite(_LED0_B, 0);
    }
}
#pragma endregion

#pragma region Misc
/**
 * Makes rover dance
 **/
void Rover::dance()
{
    int x = 30; // will run for ~30 seconds
    rainbowLights();
    while (x > 0)
    {
        steerLeft(45);
        delay(250);
        steerRight(45);
        delay(250);
        steerLeft(45);
        delay(250);
        steerRight(45);
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

            Rover::colorSet(0, rgbColor[0], rgbColor[1], rgbColor[2]);
            Rover::colorSet(1, rgbColor[0], rgbColor[1], rgbColor[2]);
            delay(5);
        }
    }
};
#pragma endregion