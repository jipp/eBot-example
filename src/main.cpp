//#define IRREMOTE

#include "Arduino.h"
#include <EBot.h>
#include <Streaming.h>

#define DISTANCE  40

EBot eBot = EBot();
unsigned long distance = 0;
bool manualMode = true;
int speed = 120;

bool checkDistance() {
  if (eBot.getDistance() < DISTANCE) {
    return false;
  }
  return true;
}

void checkRemoteCommands() {
  char getstr;

  getstr=Serial.read();
  switch (getstr) {
    case 's':
    if (manualMode) eBot.setDirection();
    break;
    case 'f':
    if (manualMode)  {
      if (checkDistance()) {
        eBot.setDirection(EBot::FORWARD);
      } else {
        eBot.setDirection();
      }
    }
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
    case 'M':
    manualMode = !manualMode;
    break;
  }
}

void setup() {
  Serial.begin(115200);
  eBot.begin();
  eBot.setSpeed(speed);
}

void loop() {
  eBot.setAngle(90);
  checkRemoteCommands();
}
