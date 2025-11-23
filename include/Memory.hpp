#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <array>

class Memory {
    public:
        Memory();

        void Write(uint16_t addr, uint8_t data);
        uint8_t Read(uint16_t addr);
    private:
        static constexpr uint16_t CHIP8_RAM_SIZE = 0xFFF; // bytes of RAM.

        std::array<uint8_t, CHIP8_RAM_SIZE> m_ram;
};

#endif