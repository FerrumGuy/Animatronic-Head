#include "jaw.h"
#include "ServoEasing.hpp"

Jaw::Jaw(int pin) {
  jawPin = pin;
}

void Jaw::init() {
  if (jawPin != -1) {
    jawServo.attach(jawPin);
  }
  
  currTime = millis();
  jawOpenDuration = random(jawOpenMin, jawOpenMax);
  lastChompTime = currTime + jawOpenDuration;
}

void Jaw::setJawOpenClose(int open, int close) {
  jawOpen = open;
  jawClose = close;
}

void Jaw::home() {
  if (jawPin == -1) return;
  
  jawServo.setEaseTo(jawClose, 60);
  synchronizeAllServosAndStartInterrupt(false);
  do {
    delay(20);
  } while (!updateAllServos());
  
  delay(250);
  
  jawServo.easeTo(jawOpen, 60);
}

void Jaw::chomp(int closeSpeed, int openSpeed, int closeDelay) {
  if (jawPin == -1) return;
  
  jawServo.setEaseTo(jawClose, closeSpeed);
  synchronizeAllServosAndStartInterrupt(false);
  do {
    delay(20);
  } while (!updateAllServos());
  
  delay(25);
  
  jawServo.setEaseTo(jawOpen, openSpeed);
  synchronizeAllServosAndStartInterrupt(false);
  do {
    delay(20);
  } while (!updateAllServos());
}
