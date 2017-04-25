// obstacle in front ->  check and decide which direction to turn

#include "Arduino.h"
#include <Streaming.h>
#include <EBot.h>

#define DISTANCE  50

EBot eBot = EBot();
char str;
unsigned long distance = 0;
bool manualModeSet = true;
int speed = 120;
int speedRotate = 130;
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

int changeAngle(int angle) {
  if (millis() - angleTimerStart > angleTimerInterval) {
    angleTimerStart = millis();
    angle += angleDelta;
    if (angle < angleStart || angle > angleEnd) {
      angleDelta = -angleDelta;
      angle += angleDelta;
    }
  }
  return angle;
}

void checkCommands(char str) {
  switch(str) {
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
    manualModeSet = !manualModeSet;
    angle = 90;
    eBot.setAngle(angle);
    break;
    case 'c':
    stateChange();
    break;
  }
}

void manualMode(char str) {
  if (checkObstacle(angle, DISTANCE) && eBot.getDirection() == EBot::FORWARD) eBot.setDirection();
  switch(str) {
    case 's':
    eBot.setDirection();
    break;
    case 'f':
    if (!checkObstacle(angle, DISTANCE)) eBot.setDirection(EBot::FORWARD);
    break;
    case 'b':
    eBot.setDirection(EBot::BACKWARD);
    break;
    case 'l':
    eBot.setDirection(EBot::TURNLEFT);
    break;
    case 'r':
    eBot.setDirection(EBot::TURNRIGHT);
    break;
    case 'L':
    eBot.setDirection(EBot::ROTATELEFT);
    break;
    case 'R':
    eBot.setDirection(EBot::ROTATERIGHT);
    break;
  }
}

void autonomousMode(int angle) {
  if (angle == 90) {
    if (!obstacleDetected && !checkObstacle(angle, DISTANCE) && eBot.getDirection() == EBot::FORWARD) {
      obstacleDetected = true;
      eBot.setSpeed(speed*1.2);
      Serial << eBot.getSpeed() << endl;
      eBot.setDirection(EBot::ROTATERIGHT);
    } else if (obstacleDetected && checkObstacle(angle, DISTANCE) && eBot.getDirection() == EBot::ROTATERIGHT) {
      obstacleDetected = false;
      eBot.setSpeed(speed);
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

void setup() {
  Serial.begin(115200);
  eBot.begin();
  eBot.setSpeed(speed);
  eBot.setDirection(EBot::FORWARD);
}

void loop() {
  str = Serial.read();
  checkCommands(str);
  if (manualModeSet) {
    manualMode(str);
  } else {
    angle = changeAngle(angle);
    autonomousMode(angle);
  }
  //Serial << "Speed " << eBot.getSpeed() << " Direction " << eBot.getDirection() << " distance " << eBot.getDistance() << " angle " << angle << endl;
}
