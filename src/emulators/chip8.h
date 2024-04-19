#ifndef CHIP8_EMULATOR_CHIP8_H
#define CHIP8_EMULATOR_CHIP8_H

#include <cstdint>
#include <stack>
#include "SDL.h"
#include <vector>
#include <random>
#include <string>
#include "../displays/simple_display.h"
#include "../extras/input_handler.h"
#include "emulator.h"

class Chip8 : Emulator {
protected:
    // Computer Parts
    uint8_t* memory;
    uint16_t program_counter;
    uint16_t index_register;
    std::stack<uint16_t> stack;
    std::vector<uint8_t> registers;

    // Timer
    uint8_t delay_timer;
    uint8_t sound_timer;

    // Display
    SimpleDisplay& display;

    // Random number generator
    std::mt19937 engine;
    std::uniform_int_distribution<> dist{ 0, 0xFF };

    // TODO Delete after implementing other chips variant
    // Options
    bool isOlder;

    // Main Operations
    bool fetch(std::string& filename);
    bool decode(uint16_t ins);

    // Memory
    void loadFont();

    // Input
    InputHandler& inputHandler;

    // Helper
    void decodeSpriteData(uint16_t position, std::vector<bool>& v);
    static std::string spriteToString(std::vector<bool>& v);

public:
    Chip8(SimpleDisplay& display, InputHandler& inputHandler, bool isOlder);
    ~Chip8() override;

    void run(std::string& filename, bool& stopSignal) final;
};

#endif