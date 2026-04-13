#include "LedMatrix.h"

#include <LedControl.h>

namespace {

constexpr uint8_t DEVICE_INDEX = 0;
constexpr unsigned long FRAME_INTERVAL_MS = 30;
constexpr unsigned long SWEEP_PERIOD_MS = 3200;

// 1 bits form an 8x8 ring used as the eye sclera.
constexpr uint8_t RING_ROWS[8] = {
	0b00111100,
	0b01100110,
	0b11000011,
	0b10000001,
	0b10000001,
	0b11000011,
	0b01100110,
	0b00111100,
};

} // namespace

EyeMatrix::EyeMatrix(uint8_t dataPin, uint8_t clkPin, uint8_t csPin, uint8_t deviceCount)
	: _dataPin(dataPin),
	  _clkPin(clkPin),
	  _csPin(csPin),
	  _deviceCount(deviceCount),
	  _lastFrameMs(0),
	  _animationStartMs(0),
	  _ditherStep(0),
	  _intensity(8),
	  _driver(nullptr) {}

void EyeMatrix::begin() {
	_driver = new LedControl(_dataPin, _clkPin, _csPin, _deviceCount);
	_driver->shutdown(DEVICE_INDEX, false);
	_driver->setIntensity(DEVICE_INDEX, _intensity);
	_driver->clearDisplay(DEVICE_INDEX);

	_animationStartMs = millis();
	_lastFrameMs = 0;
	_ditherStep = 0;
}

void EyeMatrix::setBrightness(uint8_t intensity) {
	if (intensity > 15) {
		intensity = 15;
	}

	_intensity = intensity;
	if (_driver != nullptr) {
		_driver->setIntensity(DEVICE_INDEX, _intensity);
	}
}

void EyeMatrix::update() {
	if (_driver == nullptr) {
		return;
	}

	const unsigned long now = millis();
	if ((now - _lastFrameMs) < FRAME_INTERVAL_MS) {
		return;
	}
	_lastFrameMs = now;

	const float cycle = static_cast<float>((now - _animationStartMs) % SWEEP_PERIOD_MS) /
						static_cast<float>(SWEEP_PERIOD_MS);

	// Smooth left-right-left movement over one period.
	const float sweep = 0.5f - 0.5f * cosf(2.0f * PI * cycle);
	const float pupilPos = 2.0f + (2.0f * sweep); // top-left x in [2..4]

	renderFrame(pupilPos);
	_ditherStep = (_ditherStep + 1U) & 0x0F;
}

void EyeMatrix::renderFrame(float pupilPos) {
	for (uint8_t row = 0; row < 8; ++row) {
		_driver->setRow(DEVICE_INDEX, row, RING_ROWS[row]);
	}

	const uint8_t baseX = static_cast<uint8_t>(pupilPos);
	const float blendRight = pupilPos - static_cast<float>(baseX);
	const float blendLeft = 1.0f - blendRight;

	for (uint8_t row = 3; row <= 4; ++row) {
		for (uint8_t dx = 0; dx < 2; ++dx) {
			const uint8_t leftCol = static_cast<uint8_t>(baseX + dx);
			const uint8_t rightCol = static_cast<uint8_t>(baseX + dx + 1);

			setPupilPixel(row, leftCol, blendLeft);
			setPupilPixel(row, rightCol, blendRight);
		}
	}
}

void EyeMatrix::setPupilPixel(uint8_t row, uint8_t col, float brightness) {
	if (row > 7 || col > 7) {
		return;
	}

	const uint8_t threshold = static_cast<uint8_t>(brightness * 16.0f);
	const bool on = threshold > _ditherStep;
	_driver->setLed(DEVICE_INDEX, row, col, on);
}
