// debug file for DART rover autonomous functions
// author: Sage, co-author: Aaron
// pin variables are in all CAPS, other variables are lowerCamelCase; and vars are zero-based indexed (starting from 0)
#include <Arduino.h>
#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>
#include "Rover.h"

#define STEERING 3
#define CLAW 2
#define MOTOR 4

#define L_PHOTORESISTOR A0
#define M_PHOTORESISTOR A1
#define R_PHOTORESISTOR A2

#define LED0_R 12
#define LED0_G 11
#define LED0_B 10

#define LED1_R 9
#define LED1_G 8
#define LED1_B 7

Rover rover(
    STEERING, CLAW, MOTOR,
    L_PHOTORESISTOR, R_PHOTORESISTOR, M_PHOTORESISTOR, 
    LED0_R, LED0_G, LED0_B,
    LED1_R, LED1_G, LED1_B
);

// debug mode
bool debug = true;
bool clawTest = true;

// led colors
const int white[3] = {255, 255, 255};
const int red[3] = {255, 0, 0};
const int green[3] = {0, 255, 0};
const int blue[3] = {0, 0, 255};
const int yellow[3] = {255, 255, 0};
const int off[3] = {0, 0, 0};

// misc for logics
int i; int pos;
bool finished = false;
char lastLineLocation = ' '; // L = left, R = right
int photoresistor0;
int photoresistor1;
int photoresistor2;

void setup()
{
    
    // serial monitor
    Serial.begin(115200); // baud rate
    // initial delay before loop start, allows time to place rover on line
    delay(3000); // change to 10 seconds once testing on track

    Serial.print("Running sensor debug script.\n");
    Serial.println("Starting in 3 seconds.");
    // setColor1(white[0], white[1], white[2]); // turn on floor-facing lights, set to white
    // delay(500);
    // setColor1(off[0], off[1], off[2]);

    // if (clawTest == true)
    // {
    //     servo1.write(0);
    //     Serial.println("Testing claw");
    //     delay(5000);
    // }
}

void loop()
{
    //steering servo test
    rover.turnRight(33);
    delay(3000);
    rover.turnLeft(33); //max turning angle
    delay(3000);
};