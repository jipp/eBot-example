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

bool checkDistance() {
  if (eBot.getDistance() < DISTANCE) {
    return false;
  }
  return true;
}

void checkCommands(char str) {
  switch (str) {
    case 's':
    Serial << 's' << endl;
    if (manualMode) eBot.setDirection();
    break;
    case 'f':
    Serial << 'f' << endl;
    if (manualMode)  {
      if (checkDistance()) {
        eBot.setDirection(EBot::FORWARD);
      }
    }
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
    case 'c':
    Serial << 'c' << endl;
    stateChange();
    break;
    case '-':
    Serial << '-' << endl;
    if (manualMode) speed = eBot.setSpeed(--speed);
    break;
    case '+':
    Serial << '+' << endl;
    speed += speedDelta;
    if (manualMode) speed = eBot.setSpeed(--speed);
    break;
    case 'M':
    Serial << 'M' << endl;
    manualMode = !manualMode;
    break;
  }
}

void checkObstacle() {
  if (!checkDistance()) {
    eBot.setDirection();
  }
}

void setup() {
  Serial.begin(115200);
  eBot.begin();
  eBot.setSpeed(speed);
}

void loop() {
  checkCommands(Serial.read());
  checkObstacle();
}
