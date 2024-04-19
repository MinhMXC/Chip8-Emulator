#include <cstdio>
#include "input_handler.h"

InputHandler::InputHandler(): keys(16) {}

int InputHandler::mapKeyCode(SDL_Keycode& code) {
    switch (code) {
        case SDLK_1:
            return 1;
        case SDLK_2:
            return 2;
        case SDLK_3:
            return 3;
        case SDLK_4:
            return 0xC;
        case SDLK_q:
            return 4;
        case SDLK_w:
            return 5;
        case SDLK_e:
            return 6;
        case SDLK_r:
            return 0xD;
        case SDLK_a:
            return 7;
        case SDLK_s:
            return 8;
        case SDLK_d:
            return 9;
        case SDLK_f:
            return 0xE;
        case SDLK_z:
            return 0xA;
        case SDLK_x:
            return 0;
        case SDLK_c:
            return 0xB;
        case SDLK_v:
            return 0xF;
        default:
            return -1;
    }
}

void InputHandler::handleInput(SDL_Event& e) {
    int key{ mapKeyCode(e.key.keysym.sym) };
    if (key == -1) {
        printf("Invalid Key Pressed\n");
        return;
    }

    if (e.type == SDL_KEYDOWN) {
        keys[key] = true;
    } else {
        keys[key] = false;
    }
}

bool InputHandler::isKeyPressed(uint8_t key) {
    return keys[key];
}

int InputHandler::getKeyBeingPressed() {
    for (int i{}; i < keys.size(); i++) {
        if (keys[i]) {
            return i;
        }
    }
    return -1;
}