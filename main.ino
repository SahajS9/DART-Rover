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

const int LED0_R = 10;
const int LED0_G = 9;
const int LED0_B = 8;

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
// int i;
int timer;
bool finished1 = false; // represents different phases of course
bool finished2 = false;
bool offTrack = false;
bool turningLeft;
bool turningLeftMore;
bool turningRight;
bool turningRightMore;
bool turningStraight;        // added turning* to reduce serial monitor spam and potentially save battery (less servo writes)
bool lineFollowing = false;  // mode 1
bool locatingTarget = false; // mode 2
bool alignedWithTarget = false;
char lastLineLocation = ' '; // L = left, R = right | memory to remember where line was if lost
const int slowSpeed1 = 30;
const int slowSpeed2 = 35;
const int turnRadius1 = 12;         // smaller turn radius
const int turnRadius2 = 14;         // larger turn radius
const int timeUntilOffTrack = 1000; // time until all PRs being high to think line has stopped

void setup()
{
    rover.begin();
    rover.steerStraight();
    Serial.begin(115200); // baud rate for serial monitor
    Serial.println("Starting in 5 seconds");
    rover.pixy.setLamp(0, 0); // turns off Pixy lamps
    rover.pixy.setLED(off[0], off[1], off[2]);
    rover.clawSetPos(80); // closes claw
    // start-up lights

    #pragma region Print PR values
    unsigned long int min[3] = {};
    unsigned long int max[3] = {};

    Serial.println("Sampling light region");
    rover.colorSet(1, white[0], white[1], white[2]);
    for (int i = 0; i<=100; i++) {
        static int L = 0;
        int L_raw = analogRead(L_PHOTORESISTOR);
        L = ((L * 3) + L_raw) / 4;
        min[0] += L;

        static int M = 0;
        int M_raw = analogRead(M_PHOTORESISTOR);
        M = ((M * 3) + M_raw) / 4;
        min[1] += M;

        static int R = 0;
        int R_raw = analogRead(R_PHOTORESISTOR);
        R = ((R * 3) + R_raw) / 4;
        min[2] += R;

        Serial.print(L);
        Serial.print(' ');
        Serial.print(M);
        Serial.print(' ');
        Serial.print(R);
        Serial.print(' ');
        Serial.print('\n');

        delay(10);
    }

    rover.colorSet(0, red[0], red[1], red[2]);
    delay(1500);
    rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
    delay(1500);
    rover.colorSet(0, green[0], green[1], green[2]);
    delay(1500);

    Serial.println("Sampling dark region");
    rover.colorSet(1, white[0], white[1], white[2]);
    for (int i = 0; i<=100; i++) {
        static int L = 0;
        int L_raw = analogRead(L_PHOTORESISTOR);
        L = ((L * 3) + L_raw) / 4;
        max[0] += L;

        static int M = 0;
        int M_raw = analogRead(M_PHOTORESISTOR);
        M = ((M * 3) + M_raw) / 4;
        max[1] += M;

        static int R = 0;
        int R_raw = analogRead(R_PHOTORESISTOR);
        R = ((R * 3) + R_raw) / 4;
        max[2] += R;

        Serial.print(L);
        Serial.print(' ');
        Serial.print(M);
        Serial.print(' ');
        Serial.print(R);
        Serial.print(' ');
        Serial.print('\n');

        delay(10);
    }
    rover.colorSet(0, off[0], off[1], off[2]);

    //debugging purposes
    for (int i = 0; i<=2; i++) {
        min[i] = min[i]/100;
        max[i] = max[i]/100;
    }

    Serial.print("Minimums: ");
    for (int i = 0; i<=2; i++) {
        Serial.print(min[i]);
        Serial.print(' ');
    }
    Serial.print('\n');
    Serial.print("Maximums: ");
    for (int i = 0; i<=2; i++) {
        Serial.print(max[i]);
        Serial.print(' ');
    }
    rover.calibrate(min, max);
    #pragma endregion
    delay(3000); // uncomment this line to make 5 seconds of delay once testing on track
    Serial.print("Engaging line following mode (Startup)\n");
    lineFollowing = true;
    rover.colorSet(1, white[0], white[1], white[2]); // turn on floor-facing lights, set to white
}

void loop()
{
#pragma region Completed a part of the course
    // if rover has picked up cup, move a bit forward and resume line following again
    if (finished1 == true)
    {
        lineFollowing = true;
        locatingTarget = false;
        rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
        rover.motorSet(slowSpeed2);
        delay(250); // move forward for 1/4 second then resume loop
        return;
    }
    if (finished2 == true) // when rover has finished second part of line following
    {
        for (int stopped = 0; stopped <= 0; stopped++)
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
        // Rover on track, mantain course
        while (rover.isOffLine(0) == true && rover.isOffLine(1) == false && rover.isOffLine(2) == true)
        {
            if (turningStraight == false)
            {
                rover.motorSet(slowSpeed2);
                rover.steerStraight();
                Serial.println("On-course, going straight");
                rover.colorSet(0, off[0], off[1], off[2]);
                turningStraight = true;
                turningLeft = false;
                turningRight = false;
                turningLeftMore = false;
                turningRightMore = false;
            }
        }

        // when starting to veer off, but not nearly completely off yet
        // if middle PR and left PR gets high signal
        while (rover.isOffLine(0) == false && rover.isOffLine(1) == false && rover.isOffLine(2) == true)
        {
            if (turningLeft == false)
            {
                rover.steerLeft(turnRadius1);
                rover.motorSet(slowSpeed2);
                Serial.println("Middle and left PR has low signal, turning left");
                rover.colorSet(0, red[0], red[1], red[2]);
                lastLineLocation = 'L';
                turningStraight = false;
                turningLeft = true;
                turningRight = false;
                turningLeftMore = false;
                turningRightMore = false;
            }
        }
        // if middle PR and right PR gets high signal
        while (rover.isOffLine(0) == true && rover.isOffLine(1) == false && rover.isOffLine(2) == false)
        {
            if (turningRight == false)
            {
                rover.steerRight(turnRadius1);
                rover.motorSet(slowSpeed2);
                rover.colorSet(0, green[0], green[1], green[2]);
                Serial.println("Middle and right PR has low signal, turning right");
                lastLineLocation = 'R';
                turningStraight = false;
                turningLeft = false;
                turningRight = true;
                turningLeftMore = false;
                turningRightMore = false;
            }
        }

        // only Left PR on line, turning left
        // this means track veered more than above could account for
        while (rover.isOffLine(0) == false && rover.isOffLine(1) == true && rover.isOffLine(2) == true)
        {
            if (turningLeftMore == false)
            {
                rover.motorSet(slowSpeed2);
                rover.steerLeft(turnRadius2);
                Serial.println("Low signal on left side, turning left more");
                rover.colorSet(0, red[0], red[1], red[2]);
                rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
                turningStraight = false;
                turningLeft = false;
                turningRight = false;
                turningLeftMore = true;
                turningRightMore = false;
            }
        }
        // only right PR on line, turning right
        // this means track veered more than above could account for
        while (rover.isOffLine(0) == true && rover.isOffLine(1) == true && rover.isOffLine(2) == false)
        {
            if (turningRightMore == false)
            {
                rover.motorSet(slowSpeed2);
                rover.steerRight(turnRadius2);
                Serial.println("Low signal on right side, turning right more");
                rover.colorSet(0, green[0], green[1], green[2]);
                rover.colorSet(0, yellow[0], yellow[1], yellow[2]);
                turningStraight = false;
                turningLeft = false;
                turningRight = false;
                turningLeftMore = false;
                turningRightMore = true;
            }
        }

        // All photoresistors above threshold (off line)
        // if all 3 PRs are high, rover is completely off-track, try and find it again based on which side the line was; assumed rover was on line at some point
        if (rover.isOffLine(0) == true && rover.isOffLine(1) == true && rover.isOffLine(2) == true)
        {
            timer++; // counts up each loop so if rover off track for more than one second, stop
            delay(1);
            if (timer > timeUntilOffTrack)
            {
                timer = 0;
                offTrack = true;
                rover.motorSet(0);
                rover.steerStraight();
                Serial.println("All PRs had high signal for one second, rover is probably done with track");
                turningStraight = false;
                turningLeft = false;
                turningRight = false;
                turningLeftMore = false;
                turningRightMore = false;
                rover.colorFlash(0, yellow[0], yellow[1], yellow[2], 125);
                delay(125);
            }
        }
        else
        {
            timer = 0; // resets timer if back on line or some other condition
        }

        // All photoresistors below threshold (error state)
        // this case and two side PRs being dark is also possible, but it doesn't really matter (yet) (tm)
        while (rover.isOffLine(0) == false && rover.isOffLine(2) == false && rover.isOffLine(1) == false)
        {
            Serial.println("All PRs have low signal, stopping");
            rover.motorSet(0);
            rover.steerStraight();
            rover.colorFlash(0, blue[0], blue[1], blue[2], 125);
            delay(125);

            return;
        }
    }
#pragma endregion

    // exits loop early because cup has already been picked up if this is true
    if (finished1 == true)
    {
        return;
    }

#pragma region Signature Detection(Transition)
    if (locatingTarget == false && offTrack == true)
    {
        rover.pixy.ccc.getBlocks();
        if (!rover.pixy.ccc.numBlocks)
        {
            for (int y = 1000; y > 0; y--)
            {
                rover.pixy.ccc.getBlocks();
                if (rover.pixy.ccc.numBlocks)
                {
                    y = 0;
                    Serial.println("Tilting camera");
                    delay(100);
                    return; // exits loop if block detected and stops it
                }
                rover.pixy.setServos(0, y);
            }
            return; // exits loop here if still no blocks detected
        }
        if (!(rover.isOffLine(0) == true && rover.isOffLine(1) == true && rover.isOffLine(2) == true))
        {
            Serial.println("Detected signature, but not off the line yet");
            return;
        }
        for (int i = 0; i < rover.pixy.ccc.numBlocks; i++)
        {
            if (rover.pixy.ccc.blocks[i].m_height >= 2 || rover.pixy.ccc.blocks[i].m_width >= 2) // temporarily set as an arbitrary value, needs to be whatever size is once reached end of line, this is to avoid false positives
            {
                rover.motorSet(0);
                Serial.println("Detected target while off track, switching to target locating mode");
                lineFollowing = false;
                locatingTarget = true;
                rover.clawSet(true); // opens claw for grabbing target
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
    if (locatingTarget == true)
    {
        rover.pixy.ccc.getBlocks();
        if (!rover.pixy.ccc.numBlocks)
        {
            return;
        }
        for (int i = 0; i < rover.pixy.ccc.numBlocks; i++)
        {
            int block_x = rover.pixy.ccc.blocks[i].m_x;
            int block_y = rover.pixy.ccc.blocks[i].m_y;
            if (debug == true)
            {
                rover.pixy.ccc.blocks[i].print(); // debug print block info
            }
            // x coord goes from 1 to 316, middle is 158, so center region is about from 148 to 168
            if (block_x < 148) // turn left until center if target is on left side
            {
                rover.motorSet(slowSpeed2);
                rover.steerLeft(turnRadius1);
                Serial.println("Turning left to align to target");
                alignedWithTarget = false;
            }
            else if (block_x > 168) // turn right until center if target is on right side
            {
                rover.motorSet(slowSpeed2);
                rover.steerRight(turnRadius1);
                Serial.println("Turning right to align to target");
                alignedWithTarget = false;
            }
            // y coord goes from 1 to 208, middle is 104, so center region is about from 94 to 114
            // formula to scale center coord (x) to camera tilt degrees (y) => y = 0.207*x+1
            int degreesOfChange = 0.207 * block_y + 1;
            if (block_y < 94) // tilt camera down for better vision if below middle of image
            {
                rover.pixy.setServos(0, degreesOfChange);
            }
            else if (block_y > 114) // tilt camera up for better vision if above middle of image
            {
                rover.pixy.setServos(0, degreesOfChange);
            }
            else
            {
                rover.motorSet(slowSpeed1);
                rover.steerStraight();
                alignedWithTarget = true;
                Serial.println("Aligned with target");
            }
            if (rover.pixy.ccc.blocks[i].m_width >= 200 && alignedWithTarget == true) // once close enough, stop and grab it
            {
                rover.motorSet(0);
                Serial.println("Target is aligned and within reach, grabbing");
                delay(125);
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                delay(125);
                rover.colorFlash(0, magenta[0], magenta[1], magenta[2], 125);
                delay(125);
                rover.colorFlash(0, green[0], green[1], green[2], 125);
                rover.pixy.setLED(green[0], green[1], green[2]);
                rover.clawSet(false);
                locatingTarget = false;
                finished1 = true;
                return;
            }
        }
    }
#pragma endregion

    // Serial.println("");        // spacer
}