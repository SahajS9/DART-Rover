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
// pins with PWM: 3,5,6,9,10,11
const int CLAW_SERVO = 2;
const int STEERING_SERVO = 4;
const int MOTOR = 7;

const uint8_t L_PHOTORESISTOR = A0;
const uint8_t M_PHOTORESISTOR = A1;
const uint8_t R_PHOTORESISTOR = A2;

const int LED0_R = 11;
const int LED0_G = 10;
const int LED0_B = 9;

const int LED1_R = 6;
const int LED1_G = 5;
const int LED1_B = 3;

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
bool debug = true;

// led colors
const int white[3] = {255, 255, 255};
const int red[3] = {255, 0, 0};
const int green[3] = {0, 255, 0};
const int blue[3] = {0, 0, 255};
const int yellow[3] = {255, 255, 0};
const int magenta[3] = {255, 0, 255};
const int off[3] = {0, 0, 0};

// misc for logics
int i;
bool finished = false;
bool lineFollowing = true;
bool locatingTarget = false;
bool alignedWithTarget = false;
char lastLineLocation = ' '; // L = left, R = right | memory to remember where line was if lost
const int slowSpeed = 30;    // min 30
const int turnRadius1 = 16;
const int turnRadius2 = 22;

void setup()
{
    rover.begin();
    rover.steerStraight();
    Serial.begin(115200); // baud rate for serial monitor
    Serial.println("Starting in 5 seconds");
    rover.pixy.setLamp(0, 0); // turns off Pixy lamps, not needed yet
    rover.pixy.setLED(off[0], off[1], off[2]);
    rover.clawSetPos(80); // closes claw
    // start-up lights
    rover.colorSet(0, red[0], red[1], red[2]);
    delay(1000);
    rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
    delay(1000);
    rover.colorSet(0, green[0], green[1], green[2]);
    delay(1000);
    rover.colorSet(0, off[0], off[1], off[2]);
    delay(2000); // uncomment this line to make 5 seconds of delay once testing on track
    Serial.print("Engaging line following mode (Startup)\n");
    rover.colorSet(1, white[0], white[1], white[2]); // turn on floor-facing lights, set to white
}

void loop()
{
    // Serial.println(""); // spacer
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
            rover.rainbowLights();
            delay(1000);
            rover.rainbowLights();
            // delay(3000);
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
            Serial.println("All PRs have low signal");
            rover.motorSet(0);
            rover.steerStraight();
            rover.colorFlash(0, blue[0], blue[1], blue[2], 125);
            delay(125);
            rover.colorFlash(0, blue[0], blue[1], blue[2], 125);
            delay(125);
            rover.colorFlash(0, blue[0], blue[1], blue[2], 125);
            delay(125);
            return;
        }

        // All photoresistors off line
        // if all 3 PRs are high, rover is completely off-track, try and find it again based on which side the line was; assumed rover was on line at some point
        if (rover.isOffLine(0) == true && rover.isOffLine(1) == true && rover.isOffLine(2) == true)
        {
            Serial.println("All PRs have high signal");
            rover.motorSet(0);
            rover.steerStraight();
            rover.colorFlash(0, yellow[0], yellow[1], yellow[2], 125);
            delay(125);
            rover.colorFlash(0, yellow[0], yellow[1], yellow[2], 125);
            delay(125);
            rover.colorFlash(0, yellow[0], yellow[1], yellow[2], 125);
            delay(125);
        }

        // Left PR on line, turning left
        while (rover.isOffLine(0) == false)
        {
            rover.colorSet(0, red[0], red[1], red[2]);
            Serial.println("Low signal on left side");
            // rover.motorSet(slowSpeed);
            rover.steerLeft(turnRadius1);
            lastLineLocation = 'L';
        }

        // Right PR on line, turning right
        while (rover.isOffLine(2) == false)
        {
            rover.colorSet(0, green[0], green[1], green[2]);
            Serial.println("Low signal on right side");
            // rover.motorSet(slowSpeed);
            rover.steerRight(turnRadius1);
            lastLineLocation = 'R';
        }

        // Serial.print("Line was last on side: "); // if rover was ever off-track, set by previous conditionals
        // Serial.print(lastLineLocation);
        // Serial.print("\n");

        // if middlePR gets high signal, start turning more
        // this also means track veered more than above could account for
        while (rover.isOffLine(1) == true && rover.isOffLine(0) == true && rover.isOffLine(2) == false)
        {
            Serial.println("Middle and left PR has high signal, turning more");
            rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
            rover.motorSet(slowSpeed);
            rover.steerRight(turnRadius2);
            rover.colorSet(0, green[0], green[1], green[2]);
        }
        while (rover.isOffLine(1) == true && rover.isOffLine(0) == false && rover.isOffLine(2) == true)
        {
            Serial.println("Middle and right PR has high signal, turning more");
            rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
            rover.motorSet(slowSpeed);
            rover.steerLeft(turnRadius2);
            rover.colorSet(0, red[0], red[1], red[2]);
        }

        // Rover on track, mantain course
        while (rover.isOffLine(1) == false && (rover.isOffLine(0) == true && rover.isOffLine(2) == true))
        {
            rover.motorSet(slowSpeed);
            rover.steerStraight();
            rover.colorSet(0, off[0], off[1], off[2]);
        }
        // else // if all else fails, just print the signals
        // {
        //     Serial.print(rover.isOffLine(0));
        //     Serial.print(rover.isOffLine(1));
        //     Serial.print(rover.isOffLine(2));
        //     rover.motorSet(0);
        // }
    }
#pragma endregion

#pragma region Signature Detection(Transition)
    if (locatingTarget == false)
    {
        Serial.println("Checking for signatures");
        rover.pixy.ccc.getBlocks();
        if (!rover.pixy.ccc.numBlocks)
        {
            return; // exits loop here if no blocks detected, else continues
        }
        if (!(rover.isOffLine(0) == true && rover.isOffLine(1) == true && rover.isOffLine(2) == true))
        {
            Serial.println("Detected signature, but not off the line yet");
            return;
        }
        for (i = 0; i < rover.pixy.ccc.numBlocks; i++)
        {
            if (rover.pixy.ccc.blocks[i].m_height >= 2 || rover.pixy.ccc.blocks[i].m_width >= 2) // temporarily set as an arbitrary value, needs to be whatever size is once reached end of line, this is to avoid false positives
            {
                Serial.println("Detected target, switching to target locating mode");
                lineFollowing = false;
                locatingTarget = true;
                rover.clawSet(true); // opens claw for grabbing target
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                delay(125);
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                delay(125);
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                rover.pixy.setLED(yellow[0], yellow[1], yellow[2]); // sets camera lower lamp to yellow
                // rover.pixy.setLamp(1, 1);                           // turns on upper lamps for camera
            }
        }
    }
#pragma endregion

#pragma region PixyCam Logic
    if (locatingTarget == true)
    {
        rover.pixy.ccc.getBlocks();
        if (!rover.pixy.ccc.numBlocks)
        {
            return;
        }
        for (i = 0; i < rover.pixy.ccc.numBlocks; i++)
        {
            if (debug == true)
            {
                rover.pixy.ccc.blocks[i].print(); // debug print block info
            }
            // if (rover.pixy.ccc.blocks[i].m_age < 60)
            // {
            //     Serial.println("Ignored young block");
            //     return; // exits loop if block isn't older than 60 frames (60fps, so 1 sec) (likely a false positive)
            // }
            // x coord goes from 0 to 316, middle is 158, so center region is about from 140 to 176
            if (rover.pixy.ccc.blocks[i].m_x < 140) // turn left until center if target is on left side
            {
                rover.motorSet(slowSpeed);
                rover.steerLeft(turnRadius1);
                Serial.println("Turning left to align to target");
                alignedWithTarget = false;
            }
            else if (rover.pixy.ccc.blocks[i].m_x > 176) // turn right until center if target is on right side
            {
                rover.motorSet(slowSpeed);
                rover.steerRight(turnRadius1);
                Serial.println("Turning right to align to target");
                alignedWithTarget = false;
            }
            else
            {
                rover.motorSet(slowSpeed);
                rover.steerStraight();
                alignedWithTarget = true;
                Serial.println("Aligned with target");
            }
            if (rover.pixy.ccc.blocks[i].m_width >= 110 && alignedWithTarget == true) // once close enough, stop and grab it
            {
                Serial.println("Target is aligned and within reach, grabbing");
                rover.motorSet(0);
                delay(100);
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                delay(125);
                rover.colorFlash(0, magenta[0], magenta[1], magenta[2], 125);
                delay(125);
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                rover.pixy.setLED(green[0], green[1], green[2]);
                rover.clawSet(false);
                locatingTarget = false;
                finished = true;
                return;
            }
        }
    }
#pragma endregion

    // Serial.println("-------"); // indicates reached end of loop
    // Serial.println("");        // spacer
}