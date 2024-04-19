#ifndef CHIP8_EMULATOR_SIMPLE_DISPLAY_H
#define CHIP8_EMULATOR_SIMPLE_DISPLAY_H

#include "../constants.h"

// 2D Array based Simple Display
class SimpleDisplay {
protected:
    bool** display;

public:
    SimpleDisplay() {
        display = new bool*[WIDTH];
        for (int i{}; i < WIDTH; i++) {
            display[i] = new bool[HEIGHT]();
        }
    }

    virtual ~SimpleDisplay() {
        for (int i{}; i < WIDTH; i++) {
            delete[] display[i];
        }
        delete[] display;
    }

    bool** getArrayDisplay() {
        return display;
    }

    virtual void drawPixel(int x, int y, bool isWhite) {
        display[x][y] = isWhite;
    }

    virtual bool flipPixel(int x, int y) {
        bool flag{};

        if (display[x][y]) {
            drawPixel(x, y, false);
            flag = true;
        } else{
            drawPixel(x, y, true);
        }

        return flag;
    }

    // To be used after flipping all pixels
    virtual void updateWindowSurface() {
    }

    virtual void clearScreen() {
        for (int i{}; i < WIDTH; i++) {
            for (int j{}; j < HEIGHT; j++) {
                drawPixel(i, j, false);
            }
        }
    }
};

#endif
