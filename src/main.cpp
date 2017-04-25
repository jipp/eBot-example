// obstacle in front ->  check and decide which direction to turn

#include "Arduino.h"
#include <Streaming.h>
#include <ArduinoJson.h>
#include <EBot.h>

#define DISTANCE  50

EBot eBot = EBot();
char str;
bool manualMode = false;
int speed = 120;
int speedRotate = 130;
int speedDelta = 10;
int angleStart = 30;
int angleDelta = 30;
int angleEnd = 180 - angleStart;
int angle = angleStart;
unsigned long angleTimer = 0;
unsigned long angleTimerInterval = 60;
unsigned long dataTimer = 0;
unsigned long dataTimerInterval = 1000;
int state = LOW;
bool obstacleDetected = false;
DynamicJsonBuffer jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

void sendData() {
  if (millis() - dataTimer > dataTimerInterval) {
    dataTimer = millis();
    root["manualMode"] = manualMode;
    root["direction"] = eBot.getDirection();
    root["angle"] = eBot.getAngle();
    root["obstacleDetected"] = obstacleDetected;
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

int changeAngle(int angle) {
  if (millis() - angleTimer > angleTimerInterval) {
    angleTimer = millis();
    angle += angleDelta;
    if (angle < angleStart || angle > angleEnd) {
      angleDelta = -angleDelta;
      angle += angleDelta;
    }
    eBot.setAngle();
//    eBot.setAngle(angle);
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
    manualMode = !manualMode;
    angle = 90;
    eBot.setAngle(angle);
    eBot.setDirection(EBot::FORWARD);
    break;
    case 'c':
    stateChange();
    break;
  }
}

void switchManualMode(char str) {
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

void autonomousMode(int angle) {
  if (angle == 90) {
    if (!obstacleDetected && !checkDistanceOK(DISTANCE) && eBot.getDirection() == EBot::FORWARD) {
      obstacleDetected = true;
      eBot.setSpeed(speed*1.5);
      eBot.setDirection(EBot::ROTATERIGHT);
    } else if (obstacleDetected && checkDistanceOK(DISTANCE) && eBot.getDirection() == EBot::ROTATERIGHT) {
      obstacleDetected = false;
      eBot.setSpeed(speed);
      eBot.setDirection(EBot::FORWARD);
    }
  } else if (angle < 90) {
    if (!obstacleDetected && !checkDistanceOK(DISTANCE)) {
      obstacleDetected = true;
      eBot.setDirection(EBot::TURNLEFT);
    } if (obstacleDetected && checkDistanceOK(DISTANCE) && eBot.getDirection() == EBot::TURNLEFT) {
      obstacleDetected = false;
      eBot.setDirection(EBot::FORWARD);
    }
  } else if (angle > 90) {
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
  eBot.setSpeed(speed);
  eBot.setDirection(EBot::FORWARD);
}

void loop() {
  str = Serial.read();
  sendData();
  checkCommands(str);
  if (manualMode) {
    switchManualMode(str);
  } else {
    angle = changeAngle(angle);
    autonomousMode(angle);
  }
}
