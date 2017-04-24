#include "Arduino.h"
#include <Streaming.h>
#include <EBot.h>

#define DISTANCE  40

EBot eBot = EBot();
unsigned long distance = 0;
bool manualMode = true;
int speed = 120;
int speedDelta = 10;
int angleStart = 45;
int angleDelta = 45;
int angleEnd = 180 - angleStart;
int angle = angleStart;
unsigned long angleTimer = 0;
unsigned long angleTimerInterval = 50;
unsigned long angleTimerStart = millis();
volatile int state = LOW;

void stateChange() {
  state = !state;
  digitalWrite(LED_BUILTIN, state);
}

bool checkDistance(unsigned long distance) {
  if (eBot.getDistance() < distance) {
    return false;
  }
  return true;
}

bool checkObstacle(int angle, unsigned long distance) {
  eBot.setAngle(angle);

  return checkDistance(distance);
}

void checkCommands(char str) {
  switch (str) {
    case 's':
    if (manualMode) eBot.setDirection();
    break;
    case 'f':
    if (manualMode) eBot.setDirection(EBot::FORWARD);
    break;
    case 'b':
    if (manualMode) eBot.setDirection(EBot::BACKWARD);
    break;
    case 'l':
    if (manualMode) eBot.setDirection(EBot::TURNLEFT);
    break;
    case 'r':
    if (manualMode) eBot.setDirection(EBot::TURNRIGHT);
    break;
    case 'L':
    if (manualMode) eBot.setDirection(EBot::ROTATELEFT);
    break;
    case 'R':
    if (manualMode) eBot.setDirection(EBot::ROTATERIGHT);
    break;
    case '-':
    speed -= speedDelta;
    eBot.setSpeed(speed);
    speed = eBot.getSpeed();
    break;
    case '+':
    speed += speedDelta;
    eBot.setSpeed(speed);
    speed = eBot.getSpeed();
    break;
    case 'M':
    manualMode = !manualMode;
    break;
    case 'c':
    stateChange();
    break;
  }
}

void setup() {
  Serial.begin(115200);
  eBot.begin();
  eBot.setSpeed(speed);
  angleStart = millis();
}

void loop() {
  if (millis() - angleTimerStart > angleTimerInterval) {
    angleTimerStart = millis();
    Serial << millis() << " " << angle << " " << eBot.getDistance() << endl;
    eBot.setAngle(angle);
    angle += angleDelta;
    if (angle <= angleStart || angle > angleEnd) {
      angleDelta *= -1;
      angle += angleDelta;
    }
  }
  checkCommands(Serial.read());
  if (angle == 90) {
    if (!checkObstacle(angle, DISTANCE) && eBot.getDirection() == EBot::FORWARD) eBot.setDirection();
  } else if (angle < 90) {
    if (!checkObstacle(angle, DISTANCE)) eBot.setDirection(EBot::TURNLEFT);
  } else if (angle > 90) {
    if (!checkObstacle(angle, DISTANCE)) eBot.setDirection(EBot::TURNRIGHT);
  }
}
