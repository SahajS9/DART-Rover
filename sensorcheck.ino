  // main file for DART rover autonomous functions
// author: Sage, co-author: Aaron
// pin variables are in all CAPS, other variables are lowerCamelCase; and vars are zero-based indexed (starting from 0)
// gets llbrary built for this project from Rover.h, which will get its functions from Rover.cpp

#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>
#include "Rover.h"

// pin definitions
const int STEERING_SERVO = 3;
const int CLAW_SERVO = 2;
const int MOTOR = 4;

const uint8_t L_PHOTORESISTOR = A0;
const uint8_t M_PHOTORESISTOR = A1;
const uint8_t R_PHOTORESISTOR = A2;

const int LED0_R = 10;
const int LED0_G = 9;
const int LED0_B = 8;

const int LED1_R = 7;
const int LED1_G = 6;
const int LED1_B = 5;

Rover rover(
    STEERING_SERVO,
    CLAW_SERVO,
    MOTOR,
    L_PHOTORESISTOR,
    R_PHOTORESISTOR,
    M_PHOTORESISTOR,
    LED0_R,
    LED0_G,
    LED0_B,
    LED1_R,
    LED1_G,
    LED1_B);

// debug mode
bool debug = false;

// led colors
const int white[3] = {255, 255, 255};
const int red[3] = {255, 0, 0};
const int green[3] = {0, 255, 0};
const int blue[3] = {0, 0, 255};
const int yellow[3] = {255, 180, 0};
const int off[3] = {0, 0, 0};

// misc for logics
int i;
bool finished = false;
bool lineFollowing = true;
bool locatingTarget = false;
char lastLineLocation = ' '; // L = left, R = right
bool runOnce = false;

void setup()
{
    rover.begin();
    rover.steerStraight();
    Serial.begin(115200); // baud rate for serial monitor
    Serial.println("Starting in 3 seconds");
    // start-up light
    rover.colorSet(1, red[0], red[1], red[2]);
    delay(1000);
    // delay(3000); // change to 10 seconds once testing on track
    Serial.print("Engaging sensor check suite\n");
    rover.colorSet(1, white[0], white[1], white[2]); // turn on floor-facing lights, set to white
}

void loop()
{
    // Photoresistor Test
      Serial.print("L = ");
      Serial.print(analogRead(L_PHOTORESISTOR));
      Serial.print("    M = ");
      Serial.print(analogRead(M_PHOTORESISTOR));
      Serial.print("    R = ");
      Serial.print(analogRead(R_PHOTORESISTOR));
      Serial.print("\n");

    if (runOnce == true)
    {
        rover.clawSet(false);
        delay(1000);
        rover.clawSet(true);  
        delay(5000);
        rover.motorSet(10);
        delay(3000);
        rover.motorSet(50);
        delay(3000);
        rover.motorSet(100);
        delay(10000);

        runOnce = false;
    }

    Serial.println("-------"); // indicates reached end of loop
    Serial.println("");        // spacer
}
