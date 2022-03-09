// main file for DART rover autonomous functions
// author: Sage, co-author: Aaron
// pin variables are in all CAPS, other variables are lowerCamelCase; and vars are zero-based indexed (starting from 0)
// gets llbrary built for this project from Rover.h, which will get its functions from Rover.cpp
// led group 0 is the indicators, group 1 is the underglow
// we use aircraft standards to indicate the direction of turn (i.e. red = left, green = right)

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
const int magenta[3] = {255, 0, 255};
const int off[3] = {0, 0, 0};

// misc for logics
int i;
bool finished = false;
bool lineFollowing = true;
bool locatingTarget = false;
bool alignedWithTarget = false;
char lastLineLocation = ' '; // L = left, R = right

void setup()
{
    rover.begin();
    rover.steerStraight();
    Serial.begin(115200); // baud rate for serial monitor
    Serial.println("Starting in 3 seconds");
    rover.pixy.setLamp(0, 0); // turns off Pixy lamps, not needed yet
    rover.pixy.setLED(off[0], off[1], off[2]);
    rover.clawSet(true); // closes claw
    // start-up lights
    rover.colorSet(0, red[0], red[1], red[2]);
    delay(1000);
    rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
    delay(1000);
    rover.colorSet(0, green[0], green[1], green[2]);
    delay(1000);
    delay(2000); // uncomment this line to make 5 seconds of delay once testing on track
    rover.colorSet(0, off[0], off[1], off[2]);
    Serial.print("Engaging line following mode (Startup)\n");
    rover.colorSet(1, white[0], white[1], white[2]); // turn on floor-facing lights, set to white
}

void loop()
{
#pragma region Completed Course
    // if rover has finished, stop loop
    if (finished == true)
    {
        for (int stopped = 0; i <= 0; i++)
        {
            rover.colorSet(1, off[0], off[1], off[2]); // floor lights off
            rover.motorSet(0);
            rover.steerStraight();
            Serial.println("Finished, engaging celebratory measures");
            stopped = 1; // == true
            delay(3000);
            // rover.dance();
        }
        return;
    }
#pragma endregion

#pragma region Line Follow Logic
    if (lineFollowing == true)
    {
        // All photoresistors below threshold
        while (rover.isOffLine(0) == false && rover.isOffLine(2) == false && rover.isOffLine(1) == false)
        {
            Serial.println("All PRs have low signal: Undefined state");
            rover.motorSet(0);
            rover.steerStraight();
            rover.colorFlash(0, blue[0], blue[1], blue[2], 500);
            delay(500);
            rover.colorFlash(0, blue[0], blue[1], blue[2], 500);
            delay(500);
            rover.colorFlash(0, blue[0], blue[1], blue[2], 500);
            delay(500);
            return;
        }

        // All photoresistors off line
        // if all 3 PRs are high, rover is completely off-track, try and find it again based on which side the line was; assumed rover was on line at some point
        if (rover.isOffLine(0) == true && rover.isOffLine(1) == true && rover.isOffLine(2) == true)
        {
            rover.motorSet(0);
            rover.steerStraight();
            Serial.println("Line tracking lost: Attempting to regain in 3 seconds");
            rover.colorFlash(0, yellow[0], yellow[1], yellow[2], 250);
            delay(250);
            rover.colorFlash(0, yellow[0], yellow[1], yellow[2], 250);
            delay(250);
            rover.colorFlash(0, yellow[0], yellow[1], yellow[2], 250);
            delay(250);
            delay(1000); //  delay, then lights off, then action
            if (lastLineLocation == 'L')
            {
                rover.colorSet(0, red[0], red[1], red[2]);
                rover.steerLeft(5);
                rover.motorSet(10);
                while (rover.isOffLine(0) == true && rover.isOffLine(1) == true && rover.isOffLine(2) == true)
                {
                    delay(100);
                }
            }
            if (lastLineLocation == 'R')
            {
                rover.colorSet(0, green[0], green[1], green[2]);
                rover.steerRight(5);
                rover.motorSet(10);
                while (rover.isOffLine(0) == true && rover.isOffLine(2) == true && rover.isOffLine(1) == true) // waits until back on-track, so steering will be held until
                {
                    delay(100);
                }
            }
            rover.steerStraight();
            return; // exit loop here
        }

        // Left PR on line, turning left
        if (rover.isOffLine(0) == false)
        {
            rover.colorSet(0, red[0], red[1], red[2]);
            Serial.println("Low signal on left side");
            rover.motorSet(5);
            rover.steerLeft(5);
            delay(100);
            lastLineLocation = 'L';
        }

        // Right PR on line, turning right
        if (rover.isOffLine(2) == false)
        {
            rover.colorSet(0, green[0], green[1], green[2]);
            Serial.println("Low signal on right side");
            rover.motorSet(5);
            rover.steerRight(5);
            delay(100);
            lastLineLocation = 'R';
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
        if (rover.isOffLine(1) == false && (rover.isOffLine(0) == true && rover.isOffLine(2) == true))
        {
            rover.motorSet(10);
            rover.steerStraight();
            rover.colorSet(0, off[0], off[1], off[2]);
            while (rover.isOffLine(1) == false && (rover.isOffLine(2) == true && rover.isOffLine(0) == true))
            {
                delay(100);
            }
        }
        else // if all else fails, just print the signals
        {
            Serial.print(rover.isOffLine(0));
            Serial.print(rover.isOffLine(1));
            Serial.print(rover.isOffLine(2));
        }
    }
#pragma endregion

#pragma region Signature Detection(Transition)
    rover.pixy.ccc.getBlocks();
    if (rover.pixy.ccc.numBlocks && locatingTarget == false)
    {
        for (i = 0; i < rover.pixy.ccc.numBlocks; i++)
        {
            if (debug == true)
            {
                rover.pixy.ccc.blocks[i].print(); // debug print block info
            }
            if (rover.pixy.ccc.blocks[i].m_height >= 10) // temporarily set as an arbitrary value, needs to be whatever size is once reached end of line, this is to avoid false positives
            {
                Serial.println("Detected target, switching to target locating mode");
                lineFollowing = false;
                locatingTarget = true;
                rover.clawSet(false); // opens claw for grabbing target
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                delay(125);
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                delay(125);
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                rover.pixy.setLED(yellow[0], yellow[1], yellow[2]); // sets camera lower lamp to yellow
                rover.pixy.setLamp(1, 1);                           // turns on upper lamps for camera
            }
        }
    }
#pragma endregion

#pragma region PixyCam Logic
    if (locatingTarget == true && rover.pixy.ccc.numBlocks)
    {
        for (i = 0; i < rover.pixy.ccc.numBlocks; i++)
        {
            // x coord goes from 0 to 316, middle is 158, center region from 150 to 166
            if (rover.pixy.ccc.blocks[i].m_x < 150) // turn left until center if target is on left side
            {
                while (rover.pixy.ccc.blocks[i].m_x < 150)
                {
                    rover.motorSet(5);
                    rover.steerLeft(5);
                    Serial.println("Turning left to align to target");
                }
            }
            else if (rover.pixy.ccc.blocks[i].m_x > 166) // turn right until center if target is on right side
            {
                while (rover.pixy.ccc.blocks[i].m_x > 166)
                {
                    rover.motorSet(5);
                    rover.steerRight(5);
                    Serial.println("Turning right to align to target");
                }
            }
            else
            {
                rover.motorSet(5);
                rover.steerStraight();
                alignedWithTarget = true;
                Serial.println("Aligned with target");
            }
            if (rover.pixy.ccc.blocks[i].m_height >= 100 && ) // once close enough, stop and grab it
            {
                Serial.println("Target is within reach, grabbing");
                rover.motorSet(0);
                delay(100);
                rover.colorSet(0, green[0], green[1], green[2]);
                delay(125);
                rover.colorSet(0, magenta[0], magenta[1], magenta[2]);
                delay(125);
                rover.colorSet(0, green[0], green[1], green[2]);
                rover.pixy.setLED(green[0], green[1], green[2]);
                rover.clawSet(true);
                delay(100);
            }
        }
    }
#pragma endregion

    Serial.println("-------"); // indicates reached end of loop
    Serial.println("");        // spacer
}