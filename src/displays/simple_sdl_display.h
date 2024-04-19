#ifndef CHIP8_EMULATOR_SIMPLE_SDL_DISPLAY_H
#define CHIP8_EMULATOR_SIMPLE_SDL_DISPLAY_H

#include <SDL.h>
#include "simple_display.h"

class SimpleSDLDisplay : public SimpleDisplay {
private:
    SDL_Window* window;
    SDL_Surface* surface;

    void setupGrid() {
        auto* verticals = new SDL_Rect[WIDTH - 1];
        for (int i{}; i < WIDTH - 1; i++) {
            verticals[i] = SDL_Rect{ (i + 1) * PIXEL_SIZE + i, 0, 1, SCREEN_HEIGHT };
        }

        auto* horizontals = new SDL_Rect[HEIGHT - 1];
        for (int i{}; i < HEIGHT - 1; i++) {
            horizontals[i] = SDL_Rect{ 0, (i + 1) * PIXEL_SIZE + i, SCREEN_WIDTH, 1 };
        }

        SDL_FillRects(surface, verticals, WIDTH - 1, GRID_COLOR);
        SDL_FillRects(surface, horizontals, HEIGHT - 1, GRID_COLOR);
        SDL_UpdateWindowSurface(window);

        delete[] horizontals;
        delete[] verticals;
    }

public:
    SimpleSDLDisplay(SDL_Window* window, SDL_Surface* surface): SimpleDisplay(), window{window}, surface{surface} {
        setupGrid();
    }

    void drawPixel(int x, int y, bool isWhite) override {
        SimpleDisplay::drawPixel(x, y, isWhite);

        int x_start{ x * (PIXEL_SIZE + 1) };
        int y_start{ y * (PIXEL_SIZE + 1) };
        SDL_Rect r{x_start, y_start, PIXEL_SIZE, PIXEL_SIZE};
        SDL_FillRect(surface, &r, isWhite ? 0xFFFFFFFF : 0x00000000);
    }

    bool flipPixel(int x, int y) override {
        bool flag{SimpleDisplay::flipPixel(x, y)};
        drawPixel(x, y, !flag);
        return flag;
    }

    // To be used after flipping all pixels
    void updateWindowSurface() override {
        SDL_UpdateWindowSurface(window);
    }

    void clearScreen() override {
        for (int i{}; i < WIDTH; i++) {
            for (int j{}; j < HEIGHT; j++) {
                drawPixel(i, j, false);
            }
        }

        SDL_UpdateWindowSurface(window);
    }
};

#endif
