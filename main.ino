// main file for DART rover autonomous functions
// author: Sage, co-author: Aaron
// pin variables are in all CAPS, other variables are lowerCamelCase; and vars are zero-based indexed (starting from 0)

#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>

// debug mode
bool debug = false;

// pixy object
Pixy2 pixy;

// servo 0 - steering
Servo servo0;
const int SERVO0 = 3;
// servo 1 - claw
Servo servo1;
const int SERVO1 = 2;
// motor
Servo motor0;
const int MOTOR0 = 4;

// led group 0 -- indicator lights
const int RED0 = 10;
const int GREEN0 = 9;
const int BLUE0 = 8;
// led group 1 -- underglow (floor-facing)
const int RED1 = 7;
const int GREEN1 = 6;
const int BLUE1 = 5;
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
    servo0.attach(SERVO0);
    servo1.attach(SERVO1);
    motor0.attach(MOTOR0);
    // leds - red, [], green, blue = left, (cathode), middle, right
    pinMode(RED0, OUTPUT);
    pinMode(GREEN0, OUTPUT);
    pinMode(BLUE0, OUTPUT);
    pinMode(RED1, OUTPUT);
    pinMode(GREEN1, OUTPUT);
    pinMode(BLUE1, OUTPUT);
    // pixy cam
    pixy.init();
    pixy.changeProg("ccc");
    // initial delay before loop start, allows time to place rover on line
    resetSteering();
    Serial.begin(115200); // baud rate for serial monitor
    Serial.println("Starting in 3 seconds.");
    resetSteering();
    setColor1(red[0], red[1], red[2]);
    delay(1000);
    setColor1(yellow[0], yellow[1], yellow[2]);
    delay(1000);
    setColor1(green[0], green[1], green[2]);
    delay(1000);
    // delay(3000); // change to 10 seconds once testing on track
    Serial.print("Engaging line following mode (Startup)\n");
    setColor1(white[0], white[1], white[2]); // turn on floor-facing lights, set to white
}

void loop()
{
    // if rover reached finish line, stop loop
    if (finished == true)
    {
        for (int stopped = 0; i <= 0; i++)
        {
            setColor1(off[0], off[1], off[2]); // floor lights off
            setSpeed(0);
            resetSteering();
            stopped = 1; // == true
            Serial.println("Finished");
            delay(3000);
            dance();
        }
        return;
    }
    while (leftPR() == false && rightPR() == false && midPR() == false)
    {
        Serial.println("All PRs have low signal: Undefined state");
        setSpeed(0);
        resetSteering();
        setColor0(blue[0], blue[1], blue[2]); // flash blue 3x
        delay(500);
        setColor0(off[0], off[1], off[2]);
        delay(100);
        setColor0(blue[0], blue[1], blue[2]);
        delay(500);
        setColor0(off[0], off[1], off[2]);
        delay(100);
        setColor0(blue[0], blue[1], blue[2]);
        delay(500);
        setColor0(off[0], off[1], off[2]);
        delay(100);
        delay(3000);
        return;
    }
    // if all 3 PRs are high, rover is completely off-track, try and find it again based on which side the line was; assumed rover was on line at some point
    if (leftPR() == true && rightPR() == true && midPR() == true)
    {
        setSpeed(0);
        resetSteering();
        Serial.println("Line tracking lost: Attempting to regain in 3 seconds");
        setColor0(yellow[0], yellow[1], yellow[2]); // flash yellow 3x
        delay(500);
        setColor0(off[0], off[1], off[2]);
        delay(100);
        setColor0(yellow[0], yellow[1], yellow[2]);
        delay(500);
        setColor0(off[0], off[1], off[2]);
        delay(100);
        setColor0(yellow[0], yellow[1], yellow[2]);
        delay(500);
        setColor0(off[0], off[1], off[2]);
        delay(1300); // total 3 seconds of delay, then lights off, then action
        if (lastLineLocation == 'R')
        {
            setColor0(green[0], green[1], green[2]);
            turnRight(30);
            setSpeed(10);
            while (leftPR() == true && rightPR() == true && midPR() == true) // waits until back on-track, so steering will be held until
            {
                delay(100);
            }
        }
        if (lastLineLocation == 'L')
        {
            setColor0(red[0], red[1], red[2]);
            turnLeft(30);
            setSpeed(10);
            while (leftPR() == true && rightPR() == true && midPR() == true)
            {
                delay(100);
            }
        }
        resetSteering();
        return;
    }
    // if leftPR gets low signal, correct by going left
    if (leftPR() == false)
    {
        setColor0(red[0], red[1], red[2]); // using aircraft standards to indicate direction of turn
        Serial.println("Low signal on left side");
        setSpeed(10);
        turnLeft(15);
        delay(100);
        lastLineLocation = 'L';
        // setSpeed(20);
    }
    // if rightPR gets low signal, correct by going right
    if (rightPR() == false)
    {
        setColor0(green[0], green[1], green[2]);
        Serial.println("Low signal on right side");
        setSpeed(10);
        turnRight(15);
        delay(100);
        lastLineLocation = 'R';
        // setSpeed(20);
    }
    Serial.print("Line was last on side: "); // if rover was ever off-track, set by previous conditionals | if empty, then rover was always on-track
    Serial.print(lastLineLocation);
    Serial.print("\n");
    // if middlePR gets high signal, slow down, using above logics to correct; else failsafes to below conditionals
    // this also means track veered more than above could account for
    if (midPR() == true && (rightPR() == false || leftPR() == false))
    {
        Serial.println("Middle PR has high signal: slowing down to allow more time to correct");
        setColor0(yellow[0], yellow[1], yellow[2]);
        setSpeed(10);
        delay(3000);
        return;
    }
    // this is when the rover is on-track, turn off indicators and maintain speed
    if (midPR() == false && (rightPR() == true && leftPR() == true))
    {
        setSpeed(20);
        resetSteering();
        setColor0(off[0], off[1], off[2]);
        while (midPR() == false && (rightPR() == true && leftPR() == true))
        {
            delay(100);
        }
    }
    else
    {
        Serial.print(leftPR());
        Serial.print(midPR());
        Serial.print(rightPR());
    }
    Serial.println("-------"); // indicates reached end of loop
    Serial.println("");
}

// functions

// photoresistor logic - true = high, false = low
bool leftPR() {return (analogRead(A0) < 960);}
bool midPR() {return (analogRead(A1) < 900);}
bool rightPR() {return (analogRead(A2) < 940);}

// steering control - 0 to 180 degrees range
void turnRight(int degrees)
{
    servo0.write(92 + degrees);
    Serial.print("Turning right by ");
    Serial.print(degrees);
    Serial.print(" degrees\n");
}
void turnLeft(int degrees)
{
    servo0.write(92 - degrees);
    Serial.print("Turning left by ");
    Serial.print(degrees);
    Serial.print(" degrees\n");
}
void resetSteering() // servo is set to 92deg for going straight, aligns better with steering system
{
    servo0.write(92);
    Serial.println("Steering reset");
}

// motor control
// speed 0 = 0%, speed 100 = 100% forward, speed -100 = 100% backwards
void setSpeed(int speed)
{
    speed = speed * 1.8; // since motor is servo object, 100% = 180deg
    motor0.write(speed);
    Serial.print("Speed set to ");
    Serial.print(speed);
    Serial.print("\n");
}

// led control
void setColor0(int R, int G, int B)
{
    analogWrite(RED0, R);
    analogWrite(GREEN0, G);
    analogWrite(BLUE0, B);
    Serial.println("Changed indicator color");
}
void setColor1(int R, int G, int B)
{
    analogWrite(RED1, R);
    analogWrite(GREEN1, G);
    analogWrite(BLUE1, B);
    Serial.println("Changed underglow color");
}

// misc 'fun'ctions
void dance()
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
void rainbowLights() // code from https://gist.github.com/jimsynz/766994#file-rgb_spectrum-c
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

            setColor0(rgbColor[0], rgbColor[1], rgbColor[2]);
            setColor1(rgbColor[0], rgbColor[1], rgbColor[2]);
            delay(5);
        }
    }
}
