// main file for DART rover autonomous functions
// author: Sage, co-author: Aaron
// pin variables are in all CAPS, other variables are lowerCamelCase; and vars are zero-based indexed (starting from 0)

#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>

// debug mode
bool debug = true;


// pixy object
Pixy2 pixy;

// servo 0 - steering
Servo servo0;
const int SERVO0 = 3;
// servo 1 - claw
Servo servo1;
const int SERVO1 = 2;
// int pos = 0;
// motor
Servo motor0;
const int MOTOR0 = 4;

// led group 0 -- indicator lights
const int RED0 = 12;
const int GREEN0 = 11;
const int BLUE0 = 10;
// led group 1 -- underglow (floor-facing)
const int RED1 = 9;
const int GREEN1 = 8;
const int BLUE1 = 7;
// led colors
const int white[3] = {255, 255, 255};
const int red[3] = {255, 0, 0};
const int green[3] = {0, 255, 0};
const int blue[3] = {0, 0, 255};
const int yellow[3] = {255, 255, 0};
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
    // serial monitor
    Serial.begin(115200); // baud rate
    Serial.println("Starting in 3 seconds.");
    Serial.print("Engaging line following mode. (Startup)");
    // pin attachments
    servo0.attach(SERVO0);
    servo1.attach(SERVO1);
    motor0.attach(MOTOR0);
    // pixy cam
    // pixy.init(); pixy.changeProg("ccc");
    // leds - red blue green, left (cathode) middle right
    // function uses standard RGB format
    pinMode(RED0, OUTPUT);
    pinMode(GREEN0, OUTPUT);
    pinMode(BLUE0, OUTPUT);
    pinMode(RED1, OUTPUT);
    pinMode(GREEN1, OUTPUT);
    pinMode(BLUE1, OUTPUT);

    // initial delay before loop start, allows time to place rover on line
    delay(3000); // change to 10 seconds once testing on track
}

void loop()
{
    // if rover reached finish line, stop loop
    if (finished == true)
    {
        setColor1(off[0], off[1], off[2]); // floor lights off
        return;
    }
    else
    {
        setColor1(white[0], white[1], white[2]); // turn on floor-facing lights, set to white
    }
    // delay(3000); // 3 sec delay
    // setSpeed(30); // move forward slowly
    if (debug)
    {
        photoresistor0 = analogRead(A0);
        Serial.print("pr0: ");
        Serial.print(photoresistor0);
        Serial.print("\n");
        photoresistor1 = analogRead(A1);
        Serial.print("pr1: ");
        Serial.print(photoresistor1);
        Serial.print("\n");
        photoresistor2 = analogRead(A2);
        Serial.print("pr2: ");
        Serial.print(photoresistor2);
        Serial.print("\n");
    }
    // if leftPR gets low signal, correct by going left
    if (leftPR() == false)
    {
        turnLeft(15);
        setColor0(green[0], green[1], green[2]); // using aircraft standards to indicate direction of turn
        lastLineLocation = 'L';
    }
    // if rightPR gets low signal, correct by going right
    if (rightPR() == false)
    {
        turnRight(15);
        setColor0(red[0], red[1], red[2]);
        lastLineLocation = 'R';
    }
    // if middlePR gets high signal, slow down
    if (midPR() == true)
    {
        setSpeed(10);
        Serial.println("Middle photoresistor detected high signal. Slowing down to allow more time to correct.");
    }
    if (leftPR() == true && rightPR() == true && midPR() == true)
    {
        setColor0(yellow[0], yellow[1], yellow[2]);
        setSpeed(0);
        Serial.println("Line tracking lost. Attempting to regain in 3 seconds.");
        delay(3000);
        if (lastLineLocation == 'R')
        {
            turnRight(60);
            setSpeed(20);
        }
        if (lastLineLocation == 'L')
        {
            turnLeft(60);
            setSpeed(20);
        }
    }
    if (debug)
    {
        Serial.print("Line was last on side: ");
        Serial.print(lastLineLocation);
        Serial.print("\n");
    }
}

// functions

// photoresistor logic - true = high, false = low
bool leftPR()
{
    return (analogRead(A0) > 512);
}
bool rightPR()
{
    return (analogRead(A1) > 512);
}
bool midPR()
{
    return (analogRead(A2) > 512);
}

// steering control - 0 to 180 degrees
void turnRight(int degrees)
{
    servo0.write(degrees);
    delay(15);
    Serial.print("Turning right by ");
    Serial.print(degrees);
    Serial.print("\n");
}
void turnLeft(int degrees)
{
    servo0.write(-degrees);
    delay(15);
    Serial.print("Turning left by ");
    Serial.print(degrees);
    Serial.print("\n");
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
    Serial.println("Changed color for group 0.");
}
void setColor1(int R, int G, int B)
{
    analogWrite(RED1, R);
    analogWrite(GREEN1, G);
    analogWrite(BLUE1, B);
    Serial.println("Changed color for group 1.");
}
// void setColor2(int R, int G, int B)
// {
//     analogWrite(RED2, R);
//     analogWrite(GREEN2, G);
//     analogWrite(BLUE2, B);
// }