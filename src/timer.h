#ifndef CHIP8_EMULATOR_TIMER_H
#define CHIP8_EMULATOR_TIMER_H

#include <cstdint>
#include <chrono>
#include <iostream>

class Timer {
private:
    uint8_t delay_timer;
    uint8_t sound_timer;
    // For limiting sprite drawing to 60hz
    bool nextDisplayReady;

public:
    Timer(): delay_timer{}, sound_timer{}, nextDisplayReady{false} {
    }

    void run(bool& stopSignal) {
        auto start {std::chrono::high_resolution_clock::now()};

        while (!stopSignal) {
            nextDisplayReady = true;
            if (delay_timer > 0) {
                delay_timer--;
            }
            if (sound_timer > 0) {
                sound_timer--;
            }

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(16.5ms);
        }
    }

    [[nodiscard]] uint8_t getDelayTimer() const {
        return delay_timer;
    }

    [[nodiscard]] uint8_t getSoundTimer() const {
        return sound_timer;
    }

    [[nodiscard]] bool getNextDisplayReady() const {
        return nextDisplayReady;
    }

    void setDelayTimer(uint8_t val) {
        delay_timer = val;
    }

    void setSoundTimer(uint8_t val) {
        sound_timer = val;
    }

    void setNextDisplayReady(bool val) {
        nextDisplayReady = val;
    }
};

#endif
