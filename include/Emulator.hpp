#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "Memory.hpp"
#include "RenderEngine.hpp"
#include "Audio.hpp"
#include "Processor.hpp"
#include "Consts.hpp"

#include <string>
#include <cstdint>
#include <array>

class Emulator {
    public:
        Emulator() = default;

        void InitEmulatorAudio();
        void LoadROMFromFile(const std::string& romPath);
        bool RE_IsThisPixelOnAt(uint8_t x, uint8_t y);
        void UpdateProcessorTimers();
        void DoSingleProcessorCycle();
    private:
        static constexpr uint16_t MAX_ALLOWED_ROM_SIZE = 0xFFF - PROCESSOR_INTERNAL_PROGRAM_COUNTER_START;
        static constexpr uint8_t FONTSET_SIZE = 80;
        static constexpr std::array<uint8_t, FONTSET_SIZE> FONTSET = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, //0
            0x20, 0x60, 0x20, 0x20, 0x70, //1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
            0x90, 0x90, 0xF0, 0x10, 0x10, //4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
            0xF0, 0x10, 0x20, 0x40, 0x40, //7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
            0xF0, 0x90, 0xF0, 0x90, 0x90, //A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
            0xF0, 0x80, 0x80, 0x80, 0xF0, //C
            0xE0, 0x90, 0x90, 0x90, 0xE0, //D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
            0xF0, 0x80, 0xF0, 0x80, 0x80  //F
        };
        Memory m_memory;
        RenderEngine m_renderEngine;
        Processor m_processor;
        AudioPlayer m_audioPlayer;
};

#endif
