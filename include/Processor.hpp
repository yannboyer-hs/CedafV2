#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "Audio.hpp"
#include "Memory.hpp"
#include "RenderEngine.hpp"
#include <array>
#include <cstdlib>
#include <vector>

enum class KeyState {
    PRESSED,
    NOT_PRESSED,
};

class Processor {
    public:
        Processor();

        void UpdateTimers(AudioPlayer& audioPlayer);
        uint16_t FetchNextOpcode(Memory& memory);
        void DecodeAndExecute(uint16_t opcode, Memory& memory, RenderEngine& renderEngine);
    private:
        static constexpr uint8_t REGISTERS_COUNT = 16;
        static constexpr uint8_t KEYS_COUNT = 16;
        //static constexpr uint8_t

        std::array<uint8_t, REGISTERS_COUNT> m_registers;
        std::vector<uint16_t> m_stack;
        std::array<KeyState, KEYS_COUNT> m_keys;

        uint8_t m_delayTimer;
        uint8_t m_soundTimer;
        uint16_t m_indexRegister;
        uint16_t m_programCounter;
};

#endif