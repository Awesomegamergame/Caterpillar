#ifndef Sound_H
#define Sound_H

#include <Arduino.h>

class Sound {
public:
    Sound(int pin); // Constructor
    void begin();
    void playMelody();

private:
    int _pin; // Private variable
};

#endif