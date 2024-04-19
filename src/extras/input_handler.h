#ifndef CHIP8_EMULATOR_INPUT_HANDLER_H
#define CHIP8_EMULATOR_INPUT_HANDLER_H

#include <vector>
#include "SDL_events.h"

class InputHandler {
protected:
    std::vector<bool> keys;
    static int mapKeyCode(SDL_Keycode& code);

public:
    InputHandler();

    // Check input is a valid KeyboardEvent first before using
    // Cannot cast union to one of its member type
    void handleInput(SDL_Event& e);
    bool isKeyPressed(uint8_t key);
    int getKeyBeingPressed();
};

#endif
