#include <cstdio>
#include <thread>
#include "SDL.h"
#include "SDL_events.h"
#include "constants.h"
#include "chip8.h"
#include "SDLDisplay.h"


int main(int argv, char* args[]) {
    // So that any line printed is displayed immediately
    setbuf(stdout, nullptr);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }


    SDL_Window* window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    bool quit{false};
    SDL_Event e;

    // Initialising the emulator
    SDLDisplay display{SDLDisplay(window, surface)};
    InputHandler inputHandler{};
    Timer timer{};
    Chip8 emulator{display, inputHandler, timer, false};
    std::string file{"../test_roms/Airplane.ch8"};

    std::thread computerThread(&Chip8::run, &emulator, std::ref(file), std::ref(quit));
    std::thread timerThread(&Timer::run, &timer, std::ref(quit));

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;

                // Terminating Threads
                computerThread.join();
                timerThread.join();

                // Destroy SDL Stuff
                SDL_DestroyWindow(window);
                window = nullptr;
                SDL_Quit();
            }

            if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                inputHandler.handleInput(e);
            }
        }
    }

    return 0;
}