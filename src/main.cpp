#include "Arduino.h"
#include <Streaming.h>
#include <EBot.h>

#define DISTANCE  50

EBot eBot = EBot();
unsigned long distance = 0;
bool manualMode = true;
int speed = 120;
int speedDelta = 10;
int angleStart = 30;
int angleDelta = 30;
int angleEnd = 180 - angleStart;
int angle = angleStart;
unsigned long angleTimer = 0;
unsigned long angleTimerInterval = 60;
unsigned long angleTimerStart = millis();
volatile int state = LOW;
bool obstacleDetected = false;

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
  eBot.setDirection(EBot::FORWARD);
}

void loop() {
  if (millis() - angleTimerStart > angleTimerInterval) {
    angleTimerStart = millis();
    angle += angleDelta;
    if (angle <= angleStart || angle > angleEnd) {
      angleDelta *= -1;
      angle += angleDelta;
    }
  }
  //angle = 45;
  checkCommands(Serial.read());
  if (angle == 90) {
    if (!obstacleDetected && !checkObstacle(angle, DISTANCE) && eBot.getDirection() == EBot::FORWARD) {
      obstacleDetected = true;
      eBot.setDirection(EBot::ROTATERIGHT);
    } else if (obstacleDetected && checkObstacle(angle, DISTANCE) && eBot.getDirection() == EBot::ROTATERIGHT) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  } else if (angle < 90) {
    if (!obstacleDetected && !checkObstacle(angle, DISTANCE)) {
      obstacleDetected = true;
      eBot.setDirection(EBot::TURNLEFT);
    } if (obstacleDetected && checkObstacle(angle, DISTANCE) && eBot.getDirection() == EBot::TURNLEFT) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  } else if (angle > 90) {
    if (!obstacleDetected && !checkObstacle(angle, DISTANCE)) {
      obstacleDetected = true;
      eBot.setDirection(EBot::TURNRIGHT);
    } if (obstacleDetected && checkObstacle(angle, DISTANCE) && eBot.getDirection() == EBot::TURNRIGHT) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  }
}
