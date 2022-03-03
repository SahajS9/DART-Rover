// main file for DART rover autonomous functions
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

#define LED0_R 10
#define LED0_G 9
#define LED0_B 8

#define LED1_R 7
#define LED1_G 6
#define LED1_B 5

Rover rover(
    STEERING, CLAW, MOTOR,
    L_PHOTORESISTOR, M_PHOTORESISTOR, R_PHOTORESISTOR,
    LED0_R, LED0_G, LED0_B,
    LED1_R, LED1_G, LED1_B
)

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
char lastLineLocation = ' '; // L = left, R = right
int photoresistor0;
int photoresistor1;
int photoresistor2;

void setup()
{
    rover.steerStraight();
    Serial.begin(115200); // baud rate for serial monitor
    Serial.println("Starting in 3 seconds.");

//     rover.colorSet(1, red[0], red[1], red[2]);
//     delay(1000);
//     rover.colorSet(1, yellow[0], yellow[1], yellow[2]);
//     delay(1000);
//     rover.colorSet(1, green[0], green[1], green[2]);
//     delay(1000);
//     // delay(3000); // change to 10 seconds once testing on track
//     Serial.print("Engaging line following mode (Startup)\n");
//     rover.colorSet(1, white[0], white[1], white[2]); // turn on floor-facing lights, set to white
}

void loop()
{
    #pragma region Completed Course
    // if rover reached finish line, stop loop
    if (finished == true)
    {
        for (int stopped = 0; i <= 0; i++)
        {
            rover.colorSet(1, off[0], off[1], off[2]); // floor lights off
            rover.motorSet(0);
            rover.steerStraight();
            stopped = 1; // == true
            Serial.println("Finished");
            delay(3000);
            rover.dance();
        }
        return;
    }
    #pragma endregion

    #pragma region Line Follow Logic

    //All photoresistors on the line
    while (rover.isOffLine(0) == false && rover.isOffLine(2) == false && rover.isOffLine(1) == false)
    {
        Serial.println("All PRs have low signal: Undefined state");
        rover.motorSet(0);
        rover.steerStraight();
        rover.colorSet(0, blue[0], blue[1], blue[2]); // flash blue 3x
        delay(500);
        rover.colorSet(0, off[0], off[1], off[2]);
        delay(100);
        rover.colorSet(0, blue[0], blue[1], blue[2]);
        delay(500);
        rover.colorSet(0, off[0], off[1], off[2]);
        delay(100);
        rover.colorSet(0, blue[0], blue[1], blue[2]);
        delay(500);
        rover.colorSet(0, off[0], off[1], off[2]);
        delay(100);
        delay(3000);
        return;
    }
    
    //All photoresistors off line
    // if all 3 PRs are high, rover is completely off-track, try and find it again based on which side the line was; assumed rover was on line at some point
    if (rover.isOffLine(0) == true && rover.isOffLine(2) == true && rover.isOffLine(1) == true)
    {
        rover.motorSet(0);
        rover.steerStraight();
        Serial.println("Line tracking lost: Attempting to regain in 3 seconds");
        rover.colorSet(0, yellow[0], yellow[1], yellow[2]); // flash yellow 3x
        delay(500);
        rover.colorSet(0, off[0], off[1], off[2]);
        delay(100);
        rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
        delay(500);
        rover.colorSet(0, off[0], off[1], off[2]);
        delay(100);
        rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
        delay(500);
        rover.colorSet(0, off[0], off[1], off[2]);
        delay(1300); // total 3 seconds of delay, then lights off, then action
        if (lastLineLocation == 'R')
        {
            rover.colorSet(0, green[0], green[1], green[2]);
            rover.steerRight(30);
            rover.motorSet(10);
            while (rover.isOffLine(0) == true && rover.isOffLine(2) == true && rover.isOffLine(1) == true) // waits until back on-track, so steering will be held until
            {
                delay(100);
            }
        }
        if (lastLineLocation == 'L')
        {
            rover.colorSet(0, red[0], red[1], red[2]);
            rover.steerLeft(30);
            rover.motorSet(10);
            while (rover.isOffLine(0) == true && rover.isOffLine(2) == true && rover.isOffLine(1) == true)
            {
                delay(100);
            }
        }
        rover.steerStraight();
        return;
    }
    
    //Left PR on line, turning left
    if (rover.isOffLine(0) == false)
    {
        rover.colorSet(0, red[0], red[1], red[2]); // using aircraft standards to indicate direction of turn
        Serial.println("Low signal on left side");
        rover.motorSet(10);
        rover.steerLeft(15);
        delay(100);
        lastLineLocation = 'L';
        // rover.motorSet(20);
    }
    
    //Right PR on line, turning right
    if (rover.isOffLine(2) == false)
    {
        rover.colorSet(0, green[0], green[1], green[2]);
        Serial.println("Low signal on right side");
        rover.motorSet(10);
        rover.steerRight(15);
        delay(100);
        lastLineLocation = 'R';
        // rover.motorSet(20);
    }
    
    Serial.print("Line was last on side: "); // if rover was ever off-track, set by previous conditionals | if empty, then rover was always on-track
    Serial.print(lastLineLocation);
    Serial.print("\n");

    // if middlePR gets high signal, slow down, using above logics to correct; else failsafes to below conditionals
    // this also means track veered more than above could account for
    if (rover.isOffLine(1) == true && (rover.isOffLine(2) == false || rover.isOffLine(0) == false))
    {
        Serial.println("Middle PR has high signal: slowing down to allow more time to correct");
        rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
        rover.motorSet(10);
        delay(3000);
        return;
    }
    
    // Rover on track, mantain course
    if (rover.isOffLine(1) == false && (rover.isOffLine(2) == true && rover.isOffLine(0) == true))
    {
        rover.motorSet(20);
        rover.steerStraight();
        rover.colorSet(0, off[0], off[1], off[2]);
        while (rover.isOffLine(1) == false && (rover.isOffLine(2) == true && rover.isOffLine(0) == true))
        {
            delay(100);
        }
    }
    
    else
    {
        Serial.print(rover.isOffLine(0));
        Serial.print(rover.isOffLine(1));
        Serial.print(rover.isOffLine(2));
    }
    Serial.println("-------"); // indicates reached end of loop
    Serial.println("");
    #pragma endregion
    
    //TODO: Add transitional logic between line follow and pixycam modes
    
    #pragma region PixyCam Logic
    //TODO: PixyCam Logic
    #pragma endregion
}