#include "Sound.h"


//TODO: figure out a better way to store the music data so multiple different melodies can be added into a single file and be called
// without having multiple class files for each one repeating the same code.

// Notes used by the melody (Hz)
static const uint16_t NOTE_G4 = 392;
static const uint16_t NOTE_A4 = 440;
static const uint16_t NOTE_B4 = 494;
static const uint16_t NOTE_C5 = 523;
static const uint16_t NOTE_D5 = 587;
static const uint16_t NOTE_E5 = 659;
static const uint16_t NOTE_F5 = 698;
static const uint16_t NOTE_G5 = 784;
static const uint16_t REST = 0;

static const uint16_t melody[] = {
    NOTE_A4, REST, NOTE_B4, REST, NOTE_C5, REST, NOTE_A4, REST,
    NOTE_D5, REST, NOTE_E5, REST, NOTE_D5, REST,

    NOTE_G4, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_E5, NOTE_E5, REST,
    NOTE_D5, REST,

    NOTE_G4, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_D5, NOTE_D5, REST,
    NOTE_C5, REST, NOTE_B4, NOTE_A4, REST,

    NOTE_G4, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_D5, REST,
    NOTE_B4, NOTE_A4, NOTE_G4, REST, NOTE_G4, REST, NOTE_D5, REST, NOTE_C5, REST,

    NOTE_G4, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_E5, NOTE_E5, REST,
    NOTE_D5, REST,

    NOTE_G4, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_G5, NOTE_B4, REST,
    NOTE_C5, REST, NOTE_B4, NOTE_A4, REST,

    NOTE_G4, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_C5, NOTE_D5, REST,
    NOTE_B4, NOTE_A4, NOTE_G4, REST, NOTE_G4, REST, NOTE_D5, REST, NOTE_C5, REST,

    NOTE_C5, REST, NOTE_D5, REST, NOTE_G4, REST, NOTE_D5, REST, NOTE_E5, REST,
    NOTE_G5, NOTE_F5, NOTE_E5, REST,

    NOTE_C5, REST, NOTE_D5, REST, NOTE_G4, REST
};

static const uint8_t durations[] = {
    8, 8, 8, 8, 8, 8, 8, 4,
    8, 8, 8, 8, 2, 2,

    8, 8, 8, 8, 2, 8, 8,
    2, 8,

    8, 8, 8, 8, 2, 8, 8,
    4, 8, 8, 8, 8,

    8, 8, 8, 8, 2, 8, 8,
    2, 8, 4, 8, 8, 8, 8, 8, 1, 4,

    8, 8, 8, 8, 2, 8, 8,
    2, 8,

    8, 8, 8, 8, 2, 8, 8,
    2, 8, 8, 8, 8,

    8, 8, 8, 8, 2, 8, 8,
    4, 8, 3, 8, 8, 8, 8, 8, 1, 4,

    2, 6, 2, 6, 4, 4, 2, 6, 2, 3,
    8, 8, 8, 8,

    2, 6, 2, 6, 2, 1
};

static_assert((sizeof(melody) / sizeof(melody[0])) == (sizeof(durations) / sizeof(durations[0])),
                            "melody and durations must have the same length");

Sound::Sound(int pin) {
    _pin = pin;
}

void Sound::begin() {
    pinMode(_pin, OUTPUT);
}

void Sound::playMelody() {
    const int size = sizeof(durations) / sizeof(durations[0]);

    for (int note = 0; note < size; note++) {
        const int duration = 1000 / durations[note];
        const uint16_t frequency = melody[note];

        if (frequency == REST) {
            noTone(_pin);
        } else {
            tone(_pin, frequency, duration);
        }

        const int pauseBetweenNotes = (int)(duration * 1.30);
        delay(pauseBetweenNotes);
        noTone(_pin);
    }
}