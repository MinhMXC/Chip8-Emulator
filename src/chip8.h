#ifndef CHIP8_EMULATOR_CHIP8_H
#define CHIP8_EMULATOR_CHIP8_H

#include <cstdint>
#include <stack>
#include <SDL.h>
#include <vector>
#include <string>
#include "display.h"
#include <random>
#include "input_handler.h"
#include "timer.h"

class Chip8 {
protected:
    // Computer Parts
    uint8_t* memory;
    uint16_t program_counter;
    uint16_t index_register;
    std::stack<uint16_t> stack;
    std::vector<uint8_t> registers;

    // Timer
    Timer& timer;

    // Display
    Display& display;

    // Random number generator
    std::mt19937 engine;
    std::uniform_int_distribution<> dist{ 0, 0xFF };

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
    Chip8(Display& display, InputHandler& inputHandler, Timer& timer, bool isOlder);
    ~Chip8();

    void run(std::string& filename, bool& stopSignal);

    void setIsOlder(bool value) {
        isOlder = value;
    }
};

#endif