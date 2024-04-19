#ifndef CHIP8_EMULATOR_ADVANCED_SDL_DISPLAY_H
#define CHIP8_EMULATOR_ADVANCED_SDL_DISPLAY_H

#include <cstdio>
#include <SDL.h>
#include "../constants.h"
#include "advanced_display.h"

class AdvancedSDLDisplay : public AdvancedDisplay {
protected:
    SDL_Window* window;
    SDL_Surface* surface;

    void setupGrid() {
        auto* verticals = new SDL_Rect[width - 1];
        for (int i{}; i < width - 1; i++) {
            verticals[i] = SDL_Rect{ (i + 1) * pixelSize + i, 0, 1, screenHeight };
        }

        auto* horizontals = new SDL_Rect[height - 1];
        for (int i{}; i < height - 1; i++) {
            horizontals[i] = SDL_Rect{ 0, (i + 1) * pixelSize + i, screenWidth, 1 };
        }

        SDL_FillRects(surface, verticals, width - 1, GRID_COLOR);
        SDL_FillRects(surface, horizontals, height - 1, GRID_COLOR);
        SDL_UpdateWindowSurface(window);

        delete[] horizontals;
        delete[] verticals;
    }

public:
    AdvancedSDLDisplay(SDL_Window* window, SDL_Surface* surface): AdvancedDisplay(), window{window}, surface{surface} {
        setupGrid();
    }

    void switchOperationalMode(bool val) override {
        if (isHires == val) {
            return;
        }

        AdvancedDisplay::switchOperationalMode(val);

        SDL_SetWindowSize(window, screenWidth, screenHeight);
        surface = SDL_GetWindowSurface(window);
        setupGrid();
    }

    void drawPixel(int x, int y, bool isWhite) override {
        if (x >= width || y >= height) {
            printf("Advanced Display: Draw call is out of bound. Please check operational mode!\n");
            return;
        }

        AdvancedDisplay::drawPixel(x, y, isWhite);

        int x_start{ x * (pixelSize + 1) };
        int y_start{ y * (pixelSize + 1) };
        SDL_Rect r{x_start, y_start, pixelSize, pixelSize};
        SDL_FillRect(surface, &r, isWhite ? 0xFFFFFFFF : 0x00000000);
    }

    bool flipPixel(int x, int y) override {
        bool flag{AdvancedDisplay::flipPixel(x, y)};
        drawPixel(x, y, !flag);
        return flag;
    }

    void updateWindowSurface() override {
        SDL_UpdateWindowSurface(window);
    }

    void clearScreen() override {
        for (int i{}; i < width; i++) {
            for (int j{}; j < height; j++) {
                drawPixel(i, j, false);
            }
        }

        SDL_UpdateWindowSurface(window);
    }

    void scrollUp(int size) override {
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

        SDL_UpdateWindowSurface(window);
    }

    void scrollDown(int size) override {
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

        SDL_UpdateWindowSurface(window);
    }

    void scrollRight(int size) override {
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

        SDL_UpdateWindowSurface(window);
    }

    void scrollLeft(int size) override {
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

        SDL_UpdateWindowSurface(window);
    }
};

#endif
