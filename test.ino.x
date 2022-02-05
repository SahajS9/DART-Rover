#include <SPI.h>
#include <Pixy2.h>
#include <Servo.h>

Pixy2 pixy;   // declares Pixy object
Servo servo1; // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0; // variable to store the servo position
int i;       // integer for block loop
const int delayTime = 15;
const int PIN_SERVO = 2;
const int PIN_PHOTO = 1;
// leds
const int PIN_RED = 5;
const int PIN_GREEN = 6;
const int PIN_BLUE = 7;
int photoresistor1;

void setup()
{
  // serial monitor
  Serial.begin(115200); // baud rate
  Serial.print("Starting.\n");
  // servo
  servo1.attach(PIN_SERVO);
  // pixy
  pixy.init();
  pixy.changeProg("ccc");
  // leds
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  // photoresistor
  pinMode(PIN_PHOTO, INPUT);
}

void loop()
{
  photoresistor1 = analogRead(PIN_PHOTO);
  // analogWrite(PIN_RED, 0);
  // analogWrite(PIN_GREEN, 201);
  // analogWrite(PIN_BLUE, 255);
  // delay(1000);
  int i;
  pixy.ccc.getBlocks();
  // camera get signature
  if (pixy.ccc.numBlocks)
  {
    for (i = 0; i < pixy.ccc.numBlocks; i++)
    {
      // pixy.ccc.blocks[i].print();
      if (pixy.ccc.blocks[i].m_height >= 100)
      {
        // Serial.println("Detected block with size > 100");
        pos = pos + 1;
        servo1.write(pos);
        delay(delayTime);
        setColorw(0, 255, 0);
      }
      else
      {
        // Serial.println("Not detected");
        setColorw(255, 0, 0);
        if (pos > 0)
        {
          pos = pos - 1;
          servo1.write(pos);
          delay(delayTime);  
          Serial.println(pos);
        }
      }
    }
  }
}

void setColorw(int R, int G, int B)
{
  analogWrite(PIN_RED, R);
  analogWrite(PIN_GREEN, G);
  analogWrite(PIN_BLUE, B);
}