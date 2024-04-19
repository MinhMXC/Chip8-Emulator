#include <cstdio>
#include <thread>
#include "SDL.h"
#include "SDL_events.h"
#include "constants.h"
#include "emulators/chip8.h"
#include "displays/simple_display.h"
#include "displays/simple_sdl_display.h"
#include "extras/oscillator.h"
#include "displays/advanced_sdl_display.h"
#include "emulators/schip.h"


int main(int argv, char* args[]) {
    // So that any line printed is displayed immediately
    setbuf(stdout, nullptr);



    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
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
    InputHandler inputHandler{};
    std::string file{"../test_roms/DVN8.ch8"};

    // Chip 8
    AdvancedSDLDisplay display{window, surface};
    SChip emulator{display, inputHandler};

    // SCHIP
//    AdvancedSDLDisplay display{window, surface};
//    SChip emulator{ display, inputHandler };

    std::thread cpuThread(&SChip::run, &emulator, std::ref(file), std::ref(quit));



    // Initializing the audio
    // Realised that SDL_OpenAudio creates its own thread so there was no need for a separate sound player thread
    auto callback = [](void* userdata, uint8_t* stream, int len) -> void {
        static Oscillator os{ 440.0f, 0.5f };

        auto* floatStream = (float*) stream;
        for (int i{}; i < 4096; i++) {
            floatStream[i] = Oscillator::next(os);
        }
    };

    SDL_AudioSpec spec{
        AUDIO_SAMPLE_RATE,
        AUDIO_F32,
        1,
        0,
        4096,
        0,
        0,
        callback,
        nullptr
    };

    if (SDL_OpenAudio(&spec, nullptr) < 0) {
        printf("Failed to open Audio Device: %s\n", SDL_GetError());
        return -1;
    }



    // Main Program Loop
    while (!quit) {
        while (SDL_WaitEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;

                // Terminating Threads
                printf("Terminating threads");
                cpuThread.join();
                printf("thread terminated");

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

//    AdvancedSDLDisplay advancedDisplay{ window, surface };
//    InputHandler inputHandler{};
//    SChip schip{ advancedDisplay, inputHandler };
//    advancedDisplay.switchOperationalMode(true);
//    for (int i{}; i < 32; i++) {
//        schip.memory[i] = 0xFF;
//    }
//    schip.decode(0xD010);
//    schip.decode(0xD010);
//    schip.decode(0xD010);
//    advancedDisplay.updateWindowSurface();

    while (!quit) {
        while (SDL_WaitEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;

                // Destroy SDL Stuff
                SDL_DestroyWindow(window);
                window = nullptr;
                SDL_Quit();
            }
        }
    }

    return 0;
}