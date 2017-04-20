//#define IRREMOTE

#include "Arduino.h"
#include <EBot.h>
#include <Streaming.h>

#define DISTANCE  40
#define SPEED 120

EBot eBot = EBot();
unsigned long distance = 0;
int angleStart = 30;
int angleStop = 180 - angleStart;
int angleDelta = 30;
int angle = angleStart;
unsigned long min = DISTANCE;
unsigned long minM = DISTANCE;
enum direction { STOP, FORWARD, BACKWARD, TURNLEFT, TURNRIGHT, ROTATELEFT, ROTATERIGHT };
direction move = STOP;

void drive() {
  switch (move) {
    case STOP:
    Serial << "STOP" << endl;
    eBot.stop();
    break;
    case FORWARD:
    Serial << "FORWARD" << endl;
    eBot.forward(SPEED);
    break;
    case BACKWARD:
    Serial << "BACKWARD" << endl;
    eBot.backward(SPEED);
    break;
    case TURNLEFT:
    Serial << "TURNLEFT" << endl;
    eBot.turnLeft(SPEED);
    break;
    case TURNRIGHT:
    Serial << "TURNRIGHT" << endl;
    eBot.turnRight(SPEED);
    break;
    case ROTATELEFT:
    Serial << "ROTATELEFT" << endl;
    eBot.rotateLeft(SPEED);
    break;
    case ROTATERIGHT:
    Serial << "ROTATERIGHT" << endl;
    eBot.rotateRight(SPEED);
    break;
  }
}

void checkRemoteCommands() {
  char getstr;

  getstr=Serial.read();
  switch (getstr) {
    case 's':
    move = STOP;
    break;
    case 'f':
    move = FORWARD;
    break;
    case 'b':
    move = BACKWARD;
    break;
    case 'l':
    move = TURNLEFT;
    break;
    case 'r':
    move = TURNRIGHT;
    break;
    case 'L':
    move = ROTATELEFT;
    break;
    case 'R':
    move = ROTATERIGHT;
    break;
  }
}

void setup() {
  Serial.begin(9600);
  eBot.begin();
  eBot.stop();
}

void loop() {
  checkRemoteCommands();
  drive();
  eBot.setSpeed(10);
  eBot.setDirection(EBot::FORWARD);
/*  eBot.write(90);
  distance = eBot.distance();

  if (distance < minM && angle == 90) {
    minM = distance;
    move = STOP;
  }
  if (distance > DISTANCE && angle == 90) {
    minM = DISTANCE;
    move = FORWARD;
  }

  if (distance < min && angle < 90) {
    min = distance;
    move = TURNRIGHT;
  }

  if (distance < min && angle > 90) {
    min = distance;
    move = TURNLEFT;
  }

  Serial << "angle=" << angle << " min=" << min << " distance=" << distance << endl;
  angle += angleDelta;
  if (angle > angleStop || angle < angleStart) {
    drive();
    angleDelta = -angleDelta;
    angle += angleDelta;
    min = DISTANCE;
    minM = DISTANCE;
  }
  delay(1000);
  Serial << eBot.result(); */
}
