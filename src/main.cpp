#include "Arduino.h"
#include <Streaming.h>
#include <EBot.h>

#define DISTANCE  40

EBot eBot = EBot();
unsigned long distance = 0;
bool manualMode = true;
int speed = 120;
int speedDelta = 10;
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
    Serial << 's' << endl;
    if (manualMode) eBot.setDirection();
    break;
    case 'f':
    Serial << 'f' << endl;
    if (manualMode) eBot.setDirection(EBot::FORWARD);
    break;
    case 'b':
    Serial << 'b' << endl;
    if (manualMode) eBot.setDirection(EBot::BACKWARD);
    break;
    case 'l':
    Serial << 'l' << endl;
    if (manualMode) eBot.setDirection(EBot::TURNLEFT);
    break;
    case 'r':
    Serial << 'r' << endl;
    if (manualMode) eBot.setDirection(EBot::TURNRIGHT);
    break;
    case 'L':
    Serial << 'L' << endl;
    if (manualMode) eBot.setDirection(EBot::ROTATELEFT);
    break;
    case 'R':
    Serial << 'R' << endl;
    if (manualMode) eBot.setDirection(EBot::ROTATERIGHT);
    break;
    case '-':
    Serial << '-' << endl;
    if (manualMode) {
      eBot.setSpeed(--speed);
      speed = eBot.getSpeed();
    }
    break;
    case '+':
    Serial << '+' << endl;
    if (manualMode) {
      eBot.setSpeed(++speed);
      speed = eBot.getSpeed();
    }
    case 'M':
    Serial << 'M' << endl;
    manualMode = !manualMode;
    break;
    case 'c':
    Serial << 'c' << endl;
    stateChange();
    break;
  }
}

void setup() {
  Serial.begin(115200);
  eBot.begin();
  eBot.setSpeed(speed);
}

void loop() {
  checkCommands(Serial.read());
  if (!checkObstacle(90, DISTANCE) && eBot.getDirection() == EBot::FORWARD) eBot.setDirection();
}
