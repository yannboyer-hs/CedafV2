#include "Emulator.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>
#include "Consts.hpp"

void Emulator::InitEmulatorAudio() {
    m_audioPlayer.Init();
}

void Emulator::LoadROMFromFile(const std::string& romPath) {
    std::ifstream romFile(romPath, std::ios::binary);

    if (!romFile.is_open())
        throw std::runtime_error("[ERROR] Unable to open the ROM file you provided !");
    
    romFile.seekg(0, std::ios::end); // Put the cursor at the end of the file so we can calculate the size.
    std::size_t romDataBufferSize = romFile.tellg();
    romFile.seekg(0, std::ios::beg); // Put the cursor back at the start of the file so we can read it later.

    if (romDataBufferSize > MAX_ALLOWED_ROM_SIZE)
        throw std::runtime_error("[ERROR] The ROM you tried to load is too big to fit into memory !");

    std::vector<uint8_t> romDataBuffer;

    romDataBuffer.reserve(romDataBufferSize);
    romFile.read(reinterpret_cast<char*>(romDataBuffer.data()), romDataBufferSize);
    romFile.close();

    for (std::size_t idx = 0; idx < FONTSET_SIZE; idx++) {
        m_memory.Write(idx, FONTSET[idx]);
    }

    for (std::size_t idx = 0; idx < romDataBufferSize; idx++) {
        m_memory.Write(idx + PROCESSOR_INTERNAL_PROGRAM_COUNTER_START, romDataBuffer[idx]);
    }

    std::cout << "[INFO] ROM loaded successfully into memory !\n";
    std::this_thread::sleep_for(std::chrono::seconds(4)); // Useless but I want the message above gets seen.
}

bool Emulator::RE_IsThisPixelOnAt(uint8_t x, uint8_t y) {
    return m_renderEngine.IsThisPixelOnAt(x, y);
}

void Emulator::UpdateProcessorTimers() {
    m_processor.UpdateTimers(m_audioPlayer);
}

void Emulator::DoSingleProcessorCycle() {
    uint16_t fetchedOpcode = m_processor.FetchNextOpcode(m_memory);
    m_processor.DecodeAndExecute(fetchedOpcode, m_memory, m_renderEngine);
}
