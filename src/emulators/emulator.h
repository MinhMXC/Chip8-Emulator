#ifndef CHIP8_EMULATOR_EMULATOR_H
#define CHIP8_EMULATOR_EMULATOR_H

#include <string>
#include "../extras/input_handler.h"

// Interface for Chip8, SChip and XOChip
class Emulator {

protected:
    Emulator() = default;

public:
    virtual ~Emulator() = default;
    virtual void run(std::string& filename, bool& stopSignal) {};
};

#endif
