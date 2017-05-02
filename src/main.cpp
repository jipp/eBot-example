#include "Arduino.h"
#include <Streaming.h>
#include <ArduinoJson.h>
#include <EBot.h>

const int speedDelta = 5;
const int angleStart = 30;
const unsigned long distanceMax = 50;
const unsigned long distanceMin = 20;
const unsigned long angleTimerInterval = 60;
const unsigned long dataTimerInterval = 1000;

EBot eBot = EBot();
char str;
bool isManualMode = false;
int angleDelta = 30;
const int angleEnd = 180 - angleStart;
unsigned long angleTimer = 0;
unsigned long dataTimer = 0;
int state = LOW;
bool obstacleDetected = false;
unsigned long distanceL = distanceMax;
unsigned long distanceR = distanceMax;
DynamicJsonBuffer jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

void sendData() {
  if (millis() - dataTimer > dataTimerInterval) {
    dataTimer = millis();
    root["isManualMode"] = isManualMode;
    root["direction"] = (int) eBot.getDirection();
    root["speed"] = eBot.getSpeed();
    root["angle"] = eBot.getAngle();
    root["obstacleDetected"] = obstacleDetected;
    root["state"] = state;
    root["distanceL"] = distanceL;
    root["distanceR"] = distanceR;
    root.prettyPrintTo(Serial);
    //root.printTo(Serial);
    Serial << "<eom>" << endl;
  }
}

void stateChange() {
  state = !state;
  digitalWrite(LED_BUILTIN, state);
}

bool checkDistanceOK(unsigned long distance) {
  if (eBot.getDistance() < distance) {
    return false;
  }
  return true;
}

void changeAngle() {
  int angle = eBot.getAngle();

  if (millis() - angleTimer > angleTimerInterval) {
    angleTimer = millis();
    angle += angleDelta;
    if (angle < angleStart || angle > angleEnd) {
      angleDelta = -angleDelta;
      angle += angleDelta;
      distanceL = distanceMax;
      distanceR = distanceMax;
    }
    eBot.setAngle(angle);
  }
}

void checkCommands(char str) {
  switch(str) {
    case '-':
    eBot.setSpeed(eBot.getSpeed() - speedDelta);
    break;
    case '+':
    eBot.setSpeed(eBot.getSpeed() + speedDelta);
    break;
    case 'M':
    isManualMode = !isManualMode;
    eBot.setAngle(90);
    eBot.setDirection(EBot::FORWARD);
    break;
    case 'c':
    stateChange();
    break;
  }
}

void manualMode(char str) {
  if (!checkDistanceOK(distanceMax) && eBot.getDirection() == EBot::FORWARD) eBot.setDirection();
  switch(str) {
    case 's':
    eBot.setDirection();
    break;
    case 'f':
    if (checkDistanceOK(distanceMax)) eBot.setDirection(EBot::FORWARD);
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

void autonomousMode() {
  changeAngle();
  if (eBot.getAngle() == 90) {
    if (!checkDistanceOK(distanceMin)) {
      obstacleDetected = true;
      eBot.setDirection(EBot::BACKWARD);
    } else if (!obstacleDetected && !checkDistanceOK(distanceMax) && eBot.getDirection() == EBot::FORWARD) {
      obstacleDetected = true;
      if (distanceL < distanceR) {
        eBot.setDirection(EBot::ROTATERIGHT);
      } else {
        eBot.setDirection(EBot::ROTATELEFT);
      }
    } else if (obstacleDetected && checkDistanceOK(distanceMax) && (eBot.getDirection() == EBot::ROTATELEFT || eBot.getDirection() == EBot::ROTATERIGHT || eBot.getDirection() == EBot::BACKWARD)) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  } else if (eBot.getAngle() < 90) {
    distanceR = min(distanceR, eBot.getDistance());
    if (!obstacleDetected && !checkDistanceOK(distanceMax)) {
      obstacleDetected = true;
      eBot.setDirection(EBot::TURNLEFT);
    } if (obstacleDetected && checkDistanceOK(distanceMax) && eBot.getDirection() == EBot::TURNLEFT) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  } else if (eBot.getAngle() > 90) {
    distanceL = min(distanceL, eBot.getDistance());
    if (!obstacleDetected && !checkDistanceOK(distanceMax)) {
      obstacleDetected = true;
      eBot.setDirection(EBot::TURNRIGHT);
    } if (obstacleDetected && checkDistanceOK(distanceMax) && eBot.getDirection() == EBot::TURNRIGHT) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  }
}

void setup() {
  Serial.begin(9600);
  eBot.begin();
  eBot.setDirection(EBot::FORWARD);
}

void loop() {
  str = Serial.read();
  sendData();
  checkCommands(str);
  if (isManualMode) {
    manualMode(str);
  } else {
    autonomousMode();
  }
}
