#ifndef CHIP8_EMULATOR_ADVANCED_DISPLAY_H
#define CHIP8_EMULATOR_ADVANCED_DISPLAY_H

#include "../constants.h"

class AdvancedDisplay {
protected:
    bool** display; // 2D array to store screen state so that don't need to check actual pixel value
    int width;
    int height;
    int pixelSize;
    int screenWidth;
    int screenHeight;
    bool isHires; // FALSE == LORES; TRUE == HIRES

public:
    // Constructor
    AdvancedDisplay(): width{WIDTH}, height{HEIGHT}, pixelSize{PIXEL_SIZE},
        screenWidth{SCREEN_WIDTH}, screenHeight{SCREEN_HEIGHT}, isHires{} {
        display = new bool*[HIRES_WIDTH];
        for (int i{}; i < HIRES_WIDTH; i++) {
            display[i] = new bool[HIRES_HEIGHT]();
        }
    }

    // Destructor
    virtual ~AdvancedDisplay() {
        for (int i{}; i < HIRES_WIDTH; i++) {
            delete[] display[i];
        }
        delete[] display;
    };

    // Getter
    [[nodiscard]] int getWidth() const {
        return width;
    }

    [[nodiscard]] int getHeight() const {
        return height;
    }

    // For Debugging
    bool** getDisplay() {
        return display;
    }

    void printDisplay() {
        for (int i{}; i < height; i++) {
            for (int j{}; j < width; j++) {
                printf("%d ", display[j][i] ? 1 : 0);
            }
            printf("\n");
        }
    }

    // Basic Operations
    virtual void drawPixel(int x, int y, bool isWhite) {
        display[x][y] = isWhite;
    }

    virtual void clearScreen() {
        for (int i{}; i < width; i++) {
            for (int j{}; j < height; j++) {
                drawPixel(i, j, false);
            }
        }
    }

    virtual bool flipPixel(int x, int y) {
        bool flag{};

        if (display[x][y]) {
            display[x][y] = false;
            flag = true;
        } else {
            display[x][y] = true;
        }

        return flag;
    }

    virtual void switchOperationalMode(bool val) {
        if (isHires == val) {
            return;
        }

        clearScreen(); // This ignores the SChip quirks of not clearing screen

        isHires = val;
        if (isHires) {
            width = HIRES_WIDTH;
            height = HIRES_HEIGHT;
            pixelSize = PIXEL_SIZE / 2;
            screenWidth = HIRES_SCREEN_WIDTH;
            screenHeight = HIRES_SCREEN_HEIGHT;
        } else {
            width = WIDTH;
            height = HEIGHT;
            pixelSize = PIXEL_SIZE;
            screenWidth = SCREEN_WIDTH;
            screenHeight = SCREEN_HEIGHT;
        }
    }

    virtual void updateWindowSurface() {}

    // Scroll Operations
    virtual void scrollDown(int size) {
        if (!isHires) {
            size /= 2;
        }

        for (int i{height - size - 1}; i >= 0; i--) {
            for (int j{}; j < width; j++) {
                drawPixel(j, i + size, display[j][i]);
            }
        }

        for (int i{}; i < size; i++) {
            for (int j{}; j < width; j++) {
                drawPixel(j, i, false);
            }
        }
    }

    virtual void scrollUp(int size) {
        if (!isHires) {
            size /= 2;
        }

        for (int i{size}; i < height; i++) {
            for (int j{}; j < width; j++) {
                drawPixel(j, i - size, display[j][i]);
            }
        }

        for (int i{height - size}; i < height; i++) {
            for (int j{}; j < width; j++) {
                drawPixel(j, i, false);
            }
        }
    }

    virtual void scrollLeft(int size) {
        if (!isHires) {
            size /= 2;
        }

        for (int i{}; i < height; i++) {
            for (int j{size}; j < width; j++) {
                drawPixel(j - size, i, display[j][i]);
            }
        }

        for (int i{}; i < height; i++) {
            for (int j{width - size}; j < width; j++) {
                drawPixel(j, i, false);
            }
        }
    }

    virtual void scrollRight(int size) {
        if (!isHires) {
            size /= 2;
        }

        for (int i{}; i < height; i++) {
            for (int j{width - size - 1}; j >= 0; j--) {
                drawPixel(j + size, i, display[j][i]);
            }
        }

        for (int i{}; i < height; i++) {
            for (int j{}; j < size; j++) {
                drawPixel(j, i, false);
            }
        }
    }
};

#endif
