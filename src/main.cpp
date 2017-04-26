// obstacle in front ->  check and decide which direction to turn

#include "Arduino.h"
#include <Streaming.h>
#include <ArduinoJson.h>
#include <EBot.h>

#define DISTANCE  50

EBot eBot = EBot();
char str;
bool isManualMode = false;
const int speedDelta = 5;
const int angleStart = 30;
int angleDelta = 30;
const int angleEnd = 180 - angleStart;
unsigned long angleTimer = 0;
const unsigned long angleTimerInterval = 60;
unsigned long dataTimer = 0;
const unsigned long dataTimerInterval = 1000;
int state = LOW;
bool obstacleDetected = false;
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
    }
//    eBot.setAngle();
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
  if (!checkDistanceOK(DISTANCE) && eBot.getDirection() == EBot::FORWARD) eBot.setDirection();
  switch(str) {
    case 's':
    eBot.setDirection();
    break;
    case 'f':
    if (checkDistanceOK(DISTANCE)) eBot.setDirection(EBot::FORWARD);
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
    if (!obstacleDetected && !checkDistanceOK(DISTANCE) && eBot.getDirection() == EBot::FORWARD) {
      obstacleDetected = true;
      eBot.setDirection(EBot::ROTATERIGHT);
    } else if (obstacleDetected && checkDistanceOK(DISTANCE) && (eBot.getDirection() == EBot::ROTATELEFT || eBot.getDirection() == EBot::ROTATERIGHT)) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  } else if (eBot.getAngle() < 90) {
    if (!obstacleDetected && !checkDistanceOK(DISTANCE)) {
      obstacleDetected = true;
      eBot.setDirection(EBot::TURNLEFT);
    } if (obstacleDetected && checkDistanceOK(DISTANCE) && eBot.getDirection() == EBot::TURNLEFT) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  } else if (eBot.getAngle() > 90) {
    if (!obstacleDetected && !checkDistanceOK(DISTANCE)) {
      obstacleDetected = true;
      eBot.setDirection(EBot::TURNRIGHT);
    } if (obstacleDetected && checkDistanceOK(DISTANCE) && eBot.getDirection() == EBot::TURNRIGHT) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  }
}

void setup() {
  Serial.begin(115200);
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
