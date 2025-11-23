#include "Memory.hpp"
#include <cstdlib>
#include <iostream>

Memory::Memory() {
    m_ram.fill(0x0);
}

void Memory::Write(uint16_t addr, uint8_t data) {
    if (addr > CHIP8_RAM_SIZE - 1) {
        std::cerr << "[ERROR] Memory WRITE operation failed -> Out of Range ADDR !\n";
        std::exit(EXIT_FAILURE);
    }

    m_ram[addr] = data;
}

uint8_t Memory::Read(uint16_t addr) {
    if (addr > CHIP8_RAM_SIZE - 1) {
        std::cerr << "[ERROR] Memory READ operation failed -> Out of Range ADDR !\n";
        std::exit(EXIT_FAILURE);
    }

    return m_ram[addr];
}