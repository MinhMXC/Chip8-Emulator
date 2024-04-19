#ifndef CHIP8_EMULATOR_OSCILLATOR_H
#define CHIP8_EMULATOR_OSCILLATOR_H

#include <cmath>
#include "../constants.h"

// Simple Oscillator class to programmatically generate audio
class Oscillator {
private:
    float currentStep;
    float stepSize;
    float volume;

public:
    Oscillator(float frequency, float volume) {
        this->currentStep = 0;
        this->volume = volume;
        this->stepSize = (float) (2 * M_PI) * frequency / AUDIO_SAMPLE_RATE;
    }

    static float next(Oscillator& o) {
        float val{ sinf(o.currentStep) * o.volume };
        o.currentStep += o.stepSize;
        return val;
    }

    static float squareWave(float rad) {
        if ((int) std::floor(rad / M_PI) % 2 == 0) {
            return 1;
        } else {
            return -1;
        }
    }
};

#endif
