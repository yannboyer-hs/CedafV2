#ifndef RENDER_ENGINE
#define RENDER_ENGINE

#include <array>
#include "Consts.hpp"

enum class PixelState {
    PIXEL_ON,
    PIXEL_OFF,
};

class RenderEngine {
    public:
        RenderEngine();

        bool IsThisPixelOnAt(uint8_t x, uint8_t y);
        void ChangePixelStateAt(uint8_t x, uint8_t y, PixelState newState);
        void ClearRE();
    private:
        static constexpr uint16_t RENDER_ENGINE_TABLE_SIZE = CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT;
        
        std::array<PixelState, RENDER_ENGINE_TABLE_SIZE> m_renderEngineTable;
};

#endif