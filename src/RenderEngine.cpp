#include "RenderEngine.hpp"
#include "Consts.hpp"
#include <cstdlib>
#include <iostream>

RenderEngine::RenderEngine() {
    m_renderEngineTable.fill(PixelState::PIXEL_OFF);
}

bool RenderEngine::IsThisPixelOnAt(uint8_t x, uint8_t y) {
    if (x > CHIP8_SCREEN_WIDTH || y > CHIP8_SCREEN_HEIGHT) {
        std::cerr << "[ERROR] Invalid Pixel coordinates X -> " << unsigned(x) << " Y -> " << unsigned(y) << "\n";
        std::exit(EXIT_FAILURE);
    }

    return m_renderEngineTable[y * CHIP8_SCREEN_WIDTH + x] == PixelState::PIXEL_ON;
}

void RenderEngine::ChangePixelStateAt(uint8_t x, uint8_t y, PixelState newState) {
    if (x > CHIP8_SCREEN_WIDTH || y > CHIP8_SCREEN_HEIGHT) {
        std::cerr << "[ERROR] Invalid Pixel coordinates X -> " << unsigned(x) << " Y -> " << unsigned(y) << "\n";
        std::exit(EXIT_FAILURE);
    }

    m_renderEngineTable[y * CHIP8_SCREEN_WIDTH + x] = newState;
}

void RenderEngine::ClearRE() {
    for (uint8_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
            ChangePixelStateAt(x, y, PixelState::PIXEL_OFF);
        }
    }
}