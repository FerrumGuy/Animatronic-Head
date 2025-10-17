// Jaw header file
#ifndef JAW_H
#define JAW_H

#include <Arduino.h>
#include "ServoEasing.h"

class Jaw {
  public:
    Jaw(int jawPin);
    
    ServoEasing jawServo;
    
    int jawOpen;
    int jawClose;
    
    void init();
    void setJawOpenClose(int open, int close);
    void chomp(int closeSpeed, int openSpeed, int closeDelay);
    void home();

  private:
    int jawPin;
    
    static constexpr int jawOpenShort = 300;
    static constexpr int jawOpenMin = 600;
    static constexpr int jawOpenMax = 2000;
    
    unsigned long currTime;
    unsigned long lastChompTime;
    int jawOpenDuration;
};

#endif