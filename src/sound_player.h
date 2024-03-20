#ifndef CHIP8_EMULATOR_SOUND_PLAYER_H
#define CHIP8_EMULATOR_SOUND_PLAYER_H

#include <SDL.h>
#include <cmath>
#include "timer.h"

struct Oscillator {
    float currentStep;
    float stepSize;
    float volume;

    Oscillator(float rate, float volume) {
        this->currentStep = 0;
        this->volume = volume;
        this->stepSize = (float) (2 * M_PI) / rate;
    }
};

class SoundPlayer {
private:
    Timer& timer;
    Oscillator os;

    static float next(Oscillator& o) {
        float val{ sinf(o.currentStep) * o.volume };
        o.currentStep += o.stepSize;
        return val;
    }

public:
    SoundPlayer(Timer& timer, float freq): timer{timer}, os{freq, 0.8f} {
    }

    void run(bool& stopSignal) {
        SDL_AudioSpec spec{};

        auto callback = [](void* userdata, uint8_t* stream, int len) -> void {
            static Oscillator s{ 44100 / 440.00f, 0.8f };

            float* fstream = (float*) stream;
            for (int i{}; i < 4096; i++) {
                fstream[i] = next(s);
            }
        };

        SDL_zero(spec);
        spec.format = AUDIO_F32;
        spec.channels = 1;
        spec.freq = 44100;
        spec.samples = 4096;
        spec.callback = callback;
        spec.userdata = nullptr;

        if (SDL_OpenAudio(&spec, nullptr) < 0) {
            printf("Failed to open Audio Device: %s\n", SDL_GetError());
            return;
        }

        while (!stopSignal) {
            if (timer.getSoundTimer() > 0) {
                SDL_PauseAudio(0);
            } else {
                SDL_PauseAudio(1);
            }
        }
    }
};

#endif
