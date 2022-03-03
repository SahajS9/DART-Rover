// debug file for DART rover autonomous functions
// author: Sage, co-author: Aaron
// pin variables are in all CAPS, other variables are lowerCamelCase; and vars are zero-based indexed (starting from 0)

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

Rover rover(
    STEERING, CLAW, MOTOR,
    L_PHOTORESISTOR, R_PHOTORESISTOR, M_PHOTORESISTOR, 
    LED0_R, LED0_G, LED0_B,
    LED1_R, LED1_G, LED1_B
);

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
    rover.turnRight();
    delay(3000);
    rover.turnLeft(33); //max turning angle
    delay(3000);
    // if (clawTest == true)
    // {
    //     for (pos = 0; pos <= 80; pos += 1)
    //     {
    //         servo1.write(pos);
    //         delay(20);
    //     }
    //     for (pos = 80; pos >= 0; pos -= 1)
    //     {
    //         servo1.write(pos);
    //         delay(20);
    //     }
    //     return;
    // }
    // // PR output
    // photoresistor0 = analogRead(A0);
    // Serial.print("pr0: ");
    // Serial.print(photoresistor0);
    // Serial.print(" | ");
    // delay(10);
    // photoresistor1 = analogRead(A1);
    // Serial.print("pr1: ");
    // Serial.print(photoresistor1);
    // Serial.print(" | ");
    // delay(10);
    // photoresistor2 = analogRead(A2);
    // Serial.print("pr2: ");
    // Serial.print(photoresistor2);
    // Serial.print(" | ");
    // delay(10);

    // if (leftPR() == false && rightPR() == false && midPR() == false)
    // {
    //     Serial.println("DEBUG: All PRs have low signal: Undefined state");
    //     setColor0(blue[0], blue[1], blue[2]); // flash blue 3x
    //     delay(500);
    //     setColor0(off[0], off[1], off[2]);
    //     delay(100);
    //     setColor0(blue[0], blue[1], blue[2]);
    //     delay(500);
    //     setColor0(off[0], off[1], off[2]);
    //     delay(100);
    //     setColor0(blue[0], blue[1], blue[2]);
    //     delay(500);
    //     setColor0(off[0], off[1], off[2]);
    //     delay(100);
    //     delay(3000);
    //     return;
    // }
    //     // if all 3 PRs are high, rover is completely off-track, try and find it again based on which side the line was
    //     if (leftPR() == true && rightPR() == true && midPR() == true)
    //     {
    //         Serial.println("DEBUG: Rover off-track");
    //         if (lastLineLocation == 'R')
    //         {
    //             Serial.println("DEBUG: Turned right to find line");
    //         }
    //         if (lastLineLocation == 'L')
    //         {
    //             Serial.println("DEBUG: Turned left to find line");
    //         }
    //         return;
    //     }
    // // if leftPR gets low signal, correct by going left
    // if (leftPR() == false)
    // {
    //     setColor0(red[0], red[1], red[2]); // using aircraft standards to indicate direction of turn
    //     Serial.println("DEBUG: Loss of signal on left side");
    //     lastLineLocation = 'L';
    //     // setSpeed(20);
    // }
    // // if rightPR gets low signal, correct by going right
    // if (rightPR() == false)
    // {
    //     setColor0(green[0], green[1], green[2]);
    //     Serial.println("DEBUG: Loss of signal on right side");
    //     lastLineLocation = 'R';
    //     // setSpeed(20);
    // }
    // Serial.println("DEBUG: Line was last on side: ");
    // Serial.print(lastLineLocation);
    // Serial.print("\n");
    // // if middlePR gets high signal, slow down, using above logics to correct; else failsafes to below conditionals
    // if (midPR() == true && (rightPR() == false || leftPR() == false))
    // {
    //     Serial.println("DEBUG: Middle photoresistor detected high signal");
    //     return;
    // }
    // // this is when the rover is on-track, turn off indicators and maintain speed
    // if (midPR() == false && (rightPR() == true && leftPR() == true))
    // {
    //     Serial.println("DEBUG: Rover on-track");
    //     setColor0(off[0], off[1], off[2]);
    // }
    // else
    // {
    //     Serial.print(leftPR());
    //     Serial.print(midPR());
    //     Serial.print(rightPR());
    // }
    // // pixycam
    // pixy.ccc.getBlocks();
    // if (pixy.ccc.numBlocks)
    // {
    //     for (i = 0; i < pixy.ccc.numBlocks; i++)
    //     {
    //         // pixy.ccc.blocks[i].print();
    //         if (pixy.ccc.blocks[i].m_height >= 100)
    //         {
    //             Serial.println("Detected block with size > 100");
    //             // setColor(0, 255, 0);
    //         }
    //     }
    // }

    // Serial.println("--------");
    // Serial.println("");
    // delay(1000);
}