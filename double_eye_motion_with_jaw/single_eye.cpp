#include "single_eye.h"
#include "ServoEasing.hpp"

Eye::Eye(int leftRight, int upDown, int eyeLid, int jaw)  {
  leftRightPin = leftRight;
  upDownPin = upDown;
  eyeLidPin = eyeLid;
  jawPin = jaw;
}

void Eye::init() {

  // Only attach servos if their pins are valid (not -1)
  if (leftRightPin != -1) {
    leftRightServo.attach(leftRightPin);
  }
  if (upDownPin != -1) {
    upDownServo.attach(upDownPin);
  }
  if (eyeLidPin != -1) {
    eyeLidServo.attach(eyeLidPin);
  }
  if (jawPin != -1) {
    jawServo.attach(jawPin);
  }

  currTime = millis();
  eyeOpenDuration = random(eyeOpenMin, eyeOpenMax);  //random time to keep eye open between blinks
  lastBlinkTime = currTime + eyeOpenDuration; //do this first
  eyeStillDuration = random(eyeStillMin, eyeStillMax);  //random time to keep eye still before moving
  lastMoveTime = currTime;

  currTime = millis();
  jawOpenDuration = random(jawOpenMin, jawOpenMax);  //random time to keep eye open between blinks
  lastChompTime = currTime + jawOpenDuration; //do this first
  jawStillDuration = random(jawStillMin, jawStillMax);  //random time to keep eye still before moving
  lastMoveTime = currTime;
}


void Eye::setLeftRightLowerUpperCentre(int lower, int upper, int centre) {
  leftRightLower = lower;
  leftRightUpper = upper;
  leftRightCentre = centre;
}

void Eye::setUpDownLowerUpperCentre(int lower, int upper, int centre) {
  upDownLower = lower;
  upDownUpper = upper;
  upDownCentre = centre;
}

void Eye::setEyeLidOpenClose(int open, int close){
  eyeLidOpen = open;
  eyeLidClose = close;
}

void Eye::setJawOpenClose(int open, int close){
  jawOpen = open;
  jawClose = close;
}

void Eye::moveServosTo(int leftRightServoPos, int upDownServoPos, int eyeLidServoPos, int jawServoPos, int speed) {

  leftRightServoPos = constrain(leftRightServoPos,leftRightLower,leftRightUpper);
  upDownServoPos = constrain(upDownServoPos,upDownLower,upDownUpper);
  eyeLidServoPos = constrain(eyeLidServoPos,eyeLidClose,eyeLidOpen);
  jawServoPos = constrain(jawServoPos,jawClose,jawOpen);

  if (leftRightPin != -1) {
    leftRightServo.setEaseTo(leftRightServoPos,speed);
  }
  if (upDownPin != -1) {
    upDownServo.setEaseTo(upDownServoPos,speed);
  }
  if (eyeLidPin != -1) {
    eyeLidServo.setEaseTo(eyeLidServoPos,speed);
  }
  if (jawPin != -1) {
    jawServo.setEaseTo(jawServoPos,speed);
  }
  
  synchronizeAllServosAndStartInterrupt(false);      // Do not start interrupt, because we use updateAllServos() every 20 ms below
    do {
        delay(20); 
    } while (!updateAllServos());
}

void Eye::home() {

  //home servos, move to central position with eyelid open
  moveServosTo(leftRightCentre,upDownCentre,eyeLidClose,jawClose,60);
  delay(250);
  if (eyeLidPin != -1) {
    eyeLidServo.easeTo(eyeLidOpen,60);
  }
  if (jawPin != -1) {
    jawServo.easeTo(jawOpen,60);
  }

}

void Eye::eyeMotion() {
  // while (1) {  //until exit condition, prehaps a set ammount of time, randomly defined before calling the function
    currTime = millis();
    if ((currTime - lastBlinkTime) > eyeOpenDuration) {  //blink event
      blink(random(800, 1000), random(900, 1000), 0, eyeLidOpen);
      if (!random(0, 2)) {  //sometimes don't move eye when blinking
        moveEyeBall(random(400, 600), getRndEyePos(leftRightLower, leftRightUpper, leftRightCentre, 0.4), getRndEyePos(upDownLower, upDownUpper, upDownCentre, 0.4));
        lastMoveTime = millis();
      }
      if (!random(0, 5)) {                                           //1 in x chance
        eyeOpenDuration = random(eyeOpenShort, eyeOpenShort + 100);  //short blink (double)
      } else {
        eyeOpenDuration = random(eyeOpenMin, eyeOpenMax);  //next random duration for blinking
      }
      lastBlinkTime = millis();
    }

    currTime = millis();
    if ((currTime - lastMoveTime) > eyeStillDuration) {  //move eye event
      moveEyeBall(random(100, 200), getRndEyePos(leftRightLower, leftRightUpper, leftRightCentre, 1), getRndEyePos(upDownLower, upDownUpper, upDownCentre, 1));
      lastMoveTime = millis();
    }

    //adding jaw motion to original code
    if ((currTime - lastChompTime) > jawOpenDuration) {  //chomp event
      chomp(random(800, 1000), random(900, 1000), 0, jawOpen);      
      if (!random(0, 5)) {                                           //1 in x chance
        jawOpenDuration = random(jawOpenShort, jawOpenShort + 100);  //short chomp (double)
      } else {
        jawOpenDuration = random(jawOpenMin, jawOpenMax);  //next random duration for chomping (fixed from eyeOpenDuration)
      }
      lastChompTime = millis();
    }
  // }
}

int Eye::getRndEyePos(float lowerLimit, float upperLimit, float centrePos, float centreFactor) {
  float upperRange = abs(upperLimit - centrePos);
  float lowerRange = abs(centrePos - lowerLimit);
  // int centreFactor = 0.25;

  // Serial.print("Lower Range for Random");
  // Serial.println(centrePos-(lowerRange*centreFactor));

  if (random(0, 3)) {  //x-1 in x chance
    // Serial.println("Normal");
    return random(centrePos - (lowerRange * centreFactor), centrePos + (upperRange * centreFactor));
  } else {  //1 in x chance
    // Serial.println("Rare");
    return random(lowerLimit, upperLimit);
  }
}

// Move eyeball to set position and set a single speed, both servos should finish moving at the same time for a realistic motion
void Eye::moveEyeBall(int speed, int posLR, int posUD) {
  posLR = constrain(posLR, leftRightLower, leftRightUpper);
  posUD = constrain(posUD, upDownLower, upDownUpper);

  if (leftRightPin != -1) {
    leftRightServo.setEaseTo(posLR,speed);
  }
  if (upDownPin != -1) {
    upDownServo.setEaseTo(posUD,speed);
  }
  synchronizeAllServosAndStartInterrupt(false);      // Do not start interrupt, because we use updateAllServos() every 20 ms below
  do {
    delay(20); // Optional 20ms delay. Can be less.
  } while (!updateAllServos());
}

void Eye::blink(int closeSpeed, int openSpeed, int closeDelay, int openPosition) {
  if (eyeLidPin == -1) return; // Skip if no eyelid servo
  
  eyeLidServo.setEaseTo(eyeLidClose, closeSpeed);

  synchronizeAllServosAndStartInterrupt(false);      // Do not start interrupt, because we use updateAllServos() every 20 ms below
  do {
    delay(20); // Optional 20ms delay. Can be less.
  } while (!updateAllServos());
  delay(25);
  eyeLidServo.startEaseTo(openPosition, openSpeed);
}

void Eye::chomp(int closeSpeed, int openSpeed, int closeDelay, int openPosition) {
  if (jawPin == -1) return; // Skip if no jaw servo
  
  jawServo.setEaseTo(jawClose, closeSpeed);

  synchronizeAllServosAndStartInterrupt(false);      // Do not start interrupt, because we use updateAllServos() every 20 ms below
  do {
    delay(20); // Optional 20ms delay. Can be less.
  } while (!updateAllServos());
  delay(25);
  jawServo.startEaseTo(openPosition, openSpeed);
}
