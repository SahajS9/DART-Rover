#include <SPI.h>
// #include <Pixy2.h>
#include <Servo.h>

// pin variables are in all CAPS, other variables are lowerCamelCase; and vars are zero-based indexed (starting from 0)
// photoresistors
#define PR_0 0;
#define PR_1 1;
#define PR_2 2;
int photoresistor1;
int photoresistor2;
int photoresistor3;
// pixy object
// Pixy2 pixy;
// servo 0 - steering
Servo servo0;
#define SERVO0 2;
const int delayTime = 15;
int pos = 0;
// servo 1 - claw
Servo servo1;
#define SERVO2 2;
// const int delayTime = 15;
// int pos = 0;
// motor
Servo motor0;
#define MOTOR0 3;
// led 0
#define RED0 13;
#define BLUE0 12;
#define GREEN0 11;
// led 1
#define RED1 10;
#define BLUE1 9;
#define GREEN1 8;
// led 2
#define RED2 7;
#define BLUE2 6;
#define GREEN2 5;
// misc for logics
int i;
// bool running = false;
bool finished = false;
// led colors
const int white[3] = {255, 255, 255};
const int red[3] = {255, 0, 0};
const int green[3] = {0, 255, 0};
const int blue[3] = {0, 0, 255};
const int off[3] = {0, 0, 0};


void setup()
{
    // serial monitor
    Serial.begin(115200); // baud rate
    Serial.print("Starting.\n");
    servo0.attach(SERVO0);
    servo1.attach(SERVO1);
    motor0.attach(MOTOR0);
    // pixy cam
    // pixy.init();
    // pixy.changeProg("ccc");
    // leds - red blue green, left (cathode) middle right
    // function uses standard RGB format
    pinMode(RED0, OUTPUT);
    pinMode(BLUE0, OUTPUT);
    pinMode(GREEN0, OUTPUT);
    pinMode(RED1, OUTPUT);
    pinMode(BLUE1, OUTPUT);
    pinMode(GREEN1, OUTPUT);
    pinMode(RED2, OUTPUT);
    pinMode(BLUE2, OUTPUT);
    pinMode(GREEN2, OUTPUT);
    // photoresistor
    pinMode(PR_0, INPUT);
    pinMode(PR_1, INPUT);
    pinMode(PR_2, INPUT);

    // initial delay before loop start
    delay(10000); // 10 seconds
}

void loop()
{
    // if rover reached finish, stop loop
    if (finished == true)
    {return;}
    // turn on floor-facing lights, set to white
    setColor2(white[0], white[1], white[2]);

    // if leftPR gets low signal, correct by going left
    
    // if rightPR gets low signal, correct by going right
    // if middlePR gets high signal, slow down
}


// functions

// photoresistor logic - true = high, false = low
bool leftPR()
{

    return (analogRead(PR_0) > 512);
}
bool rightPR()
{
    analogRead(PR_1);
    bool result = (PR_1 > 512) ? true : false;
}
bool midPR()
{
    analogRead(PR_2);
    bool result = (PR_2 > 512) ? true : false;
}
// steering control
void turnRight(int degrees)
{
    servo0.write(degrees);
    delay(delayTime);
}
void turnLeft(int degrees)
{
    servo0.write(-degrees);
    delay(delayTime);
}

// motor control
// speed 90 = 0%, speed 180 = 100% forward
void setSpeed(speed)
{
    motor0.write(speed);
}

// led control
void setColor0(int R, int G, int B)
{
    analogWrite(RED0, R);
    analogWrite(GREEN0, G);
    analogWrite(BLUE0, B);
}
void setColor1(int R, int G, int B)
{
    analogWrite(RED1, R);
    analogWrite(GREEN1, G);
    analogWrite(BLUE1, B);
}
void setColor2(int R, int G, int B)
{
    analogWrite(RED2, R);
    analogWrite(GREEN2, G);
    analogWrite(BLUE2, B);
}