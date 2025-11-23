#include <SDL_video.h>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include "Consts.hpp"
#include "Emulator.hpp"

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "[ERROR] No ROM provided !\n";
        std::cerr << "[INFO] Usage : ./CedafV2 my_game.ch8/my_game.rom\n";
        return EXIT_FAILURE;
    }

    Emulator chip8Emu;

    const std::string romPath = std::string(argv[1]);

    try {
        chip8Emu.InitEmulatorAudio();
        chip8Emu.LoadROMFromFile(romPath);
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("CEDAFV2 - Chip8 Emulator by Yann BOYER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "[ERROR] Unable to initialize the window !\n";
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (renderer == nullptr) {
        std::cerr << "[ERROR] Unable to inialize the renderer !\n";
        return EXIT_FAILURE;
    }

    uint8_t divCycles = 0;
    bool isRunning = true;
    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_KEYDOWN:
                    break;
                case SDL_KEYUP:
                    break;
                default: break;
            }
        }

        try {
            chip8Emu.DoSingleProcessorCycle();
            divCycles++;
        } catch (std::exception& e) {
            std::cerr << e.what() << "\n";
            return EXIT_FAILURE;
        }

        // Draw
        for (uint8_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
            for (uint8_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
                SDL_Rect pixel = { x * PIXEL_SCALE_FACTOR, y * PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR };
                if (chip8Emu.RE_IsThisPixelOnAt(x, y))
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                else
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &pixel);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer); // Prevent slowdowns...

        if (divCycles == TIMER_DIVISION_CLOCK) {
            chip8Emu.UpdateProcessorTimers();
            divCycles = 0;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(CPU_CLOCK_DELAY));
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
