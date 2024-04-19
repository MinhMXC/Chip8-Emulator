#ifndef CHIP8_EMULATOR_SCHIP_H
#define CHIP8_EMULATOR_SCHIP_H

#include <cstdint>
#include <stack>
#include "SDL.h"
#include <vector>
#include <random>
#include <string>
#include "../displays/advanced_display.h"
#include "../extras/input_handler.h"
#include "emulator.h"

class SChip : Emulator {
public:
    // Computer Parts
    uint8_t* memory;
    uint16_t program_counter;
    uint16_t index_register;
    std::stack<uint16_t> stack;
    std::vector<uint8_t> registers;
    std::vector<uint8_t> flags;

    // Timer
    uint8_t delay_timer;
    uint8_t sound_timer;

    // Display
    AdvancedDisplay& display;

    // Random number generator
    std::mt19937 engine;
    std::uniform_int_distribution<> dist{ 0, 0xFF };

    // Main Operations
    bool fetch(std::string& filename) const;
    bool decode(uint16_t ins);

    // Memory
    void loadFont() const;

    // Input
    InputHandler& inputHandler;

    // Helper
    void decodeSmallSprite(uint16_t position, std::vector<bool>& v) const;
    void decodeBigSprite(uint16_t position, std::vector<bool>& v) const;
    static std::string spriteToString(std::vector<bool>& v);

public:
    SChip(AdvancedDisplay& display, InputHandler& inputHandler);
    ~SChip() override;

    void run(std::string& filename, bool& stopSignal) override;
};

#endif
