#include "Arduino.h"

#include <EBot.h>

EBot eBot = EBot();

int ABS = 150;
int rightDistance = 0,leftDistance = 0,middleDistance = 0 ;

void setup() {
  Serial.begin(115200);
  eBot.move(true);
  eBot.stop();
}

void loop() {
  eBot.servoRotate(90);
  delay(500);
  middleDistance = eBot.distance();
  #ifdef send
  Serial.print("middleDistance=");
  Serial.println(middleDistance);
  #endif

  if(middleDistance<=20) {
    eBot.stop();
    delay(500);
    eBot.servoRotate(5);
    delay(1000);
    rightDistance = eBot.distance();

    #ifdef send
    Serial.print("rightDistance=");
    Serial.println(rightDistance);
    #endif

    delay(500);
    eBot.servoRotate(90);
    delay(1000);
    eBot.servoRotate(180);
    delay(1000);
    leftDistance = eBot.distance();

    #ifdef send
    Serial.print("leftDistance=");
    Serial.println(leftDistance);
    #endif

    delay(500);
    eBot.servoRotate(90);
    delay(1000);
    if(rightDistance>leftDistance) {
      eBot.move(true);
      eBot.barkRight();
      delay(360);
    }
    else if(rightDistance<leftDistance) {
      eBot.barkLeft();
      delay(360);
    }
    else if((rightDistance<=20)||(leftDistance<=20)) {
      eBot.backward();
      delay(180);
    } else {
      eBot.forward();
    }
  } else {
    eBot.forward();
  }
}
