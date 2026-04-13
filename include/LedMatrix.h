#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <Arduino.h>

class LedControl;

class EyeMatrix {
public:
    EyeMatrix(uint8_t dataPin, uint8_t clkPin, uint8_t csPin, uint8_t deviceCount = 1);

    void begin();
    void setBrightness(uint8_t intensity);
    void update();

private:
    void renderFrame(float pupilPos);
    void setPupilPixel(uint8_t row, uint8_t col, float brightness);

    uint8_t _dataPin;
    uint8_t _clkPin;
    uint8_t _csPin;
    uint8_t _deviceCount;

    unsigned long _lastFrameMs;
    unsigned long _animationStartMs;
    uint8_t _ditherStep;
    uint8_t _intensity;
    LedControl* _driver;
};

#endif