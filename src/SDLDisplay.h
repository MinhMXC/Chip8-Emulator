#ifndef CHIP8_EMULATOR_SDLDISPLAY_H
#define CHIP8_EMULATOR_SDLDISPLAY_H

#include <SDL.h>
#include <cstdio>
#include "display.h"
#include "constants.h"

class SDLDisplay final : public Display {
private:
    SDL_Window* window;
    SDL_Surface* surface;

public:
    SDLDisplay(SDL_Window* window, SDL_Surface* surface): window{window}, surface{surface} {
        setupGrid();
    }

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

    void drawPixel(int x, int y, bool isWhite) override {
        int x_start{ x * (PIXEL_SIZE + 1) };
        int y_start{ y * (PIXEL_SIZE + 1) };
        SDL_Rect r{x_start, y_start, PIXEL_SIZE, PIXEL_SIZE};
        SDL_FillRect(surface, &r, isWhite ? 0xFFFFFFFF : 0x00000000);
    }

    bool flipPixel(int x, int y) override {
        bool flag{};

        const uint8_t bpp{ surface->format->BytesPerPixel };

        // Account for Grid
        int x_start{ x * (PIXEL_SIZE + 1) };
        int y_start{ y * (PIXEL_SIZE + 1) };
        const uint8_t* p { (uint8_t*)surface->pixels + y_start * surface->pitch + x_start * bpp };

        SDL_Color color{ 0x00, 0x00, 0x00, 0xFF };
        SDL_GetRGBA(*(uint32_t*)p, surface->format, &color.r, &color.g, &color.b, &color.a);

        if ((color.r == 0x00) && (color.g == 0x00) && (color.b == 0x00)) {
            drawPixel(x, y, true);
        } else if ((color.r == 0xFF) && (color.g == 0xFF) && (color.b == 0xFF)) {
            flag = true;
            drawPixel(x, y, false);
        } else {
            printf("Invalid pixel value\n");
        }

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
