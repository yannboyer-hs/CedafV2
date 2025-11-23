#include "Processor.hpp"
#include "Consts.hpp"
#include <random>
#include <stdexcept>
#include <format>

Processor::Processor() {
    m_delayTimer = 0;
    m_soundTimer = 0;
    m_programCounter = PROCESSOR_INTERNAL_PROGRAM_COUNTER_START;

    m_registers.fill(0x0);
    m_stack.resize(0x0);
    m_keys.fill(KeyState::NOT_PRESSED);
}

void Processor::UpdateTimers(AudioPlayer& audioPlayer) {
    if (m_delayTimer > 0x0) m_delayTimer--;
    if (m_soundTimer > 0x0) {
        m_soundTimer--;

        if (m_soundTimer == 0x1) audioPlayer.PlayBeepSound();
    }
}

uint16_t Processor::FetchNextOpcode(Memory& memory) {
    uint16_t msb = memory.Read(m_programCounter);
    uint16_t lsb = memory.Read(m_programCounter + 1);

    return msb << 8 | lsb;
}

void Processor::DecodeAndExecute(uint16_t opcode, Memory& memory, RenderEngine& renderEngine) {
    // Hm, why recomputing ALL these values every cycle ? Only a few are used every cycle.
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t nn = opcode & 0x00FF;
    uint8_t n = opcode & 0x000F;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0:
                    // CLS
                    renderEngine.ClearRE();
                    m_programCounter += 2;
                    break;
                case 0x00EE:
                    // RET
                    m_programCounter = m_stack.back();
                    m_stack.pop_back();
                    m_programCounter += 2;
                    break;
                default:
                    throw std::runtime_error(std::format("[ERROR] Unknown Op -> {:#06x}", opcode));
                    break;
            }
            break;
        case 0x1000:
            // JP addr
            m_programCounter = nnn;
            break;
        case 0x2000:
            // CALL addr
            m_stack.push_back(m_programCounter);
            m_programCounter = nnn;
            break;
        case 0x3000:
            // SE Vx, byte
            if (m_registers[x] == nn) m_programCounter += 4;
            else m_programCounter += 2;
            break;
        case 0x4000:
            // SNE Vx, byte
            if (m_registers[x] != nn) m_programCounter += 4;
            else m_programCounter += 2;
            break;
        case 0x5000:
            // SE Vx, Vy
            if (m_registers[x] == m_registers[y]) m_programCounter += 4;
            else m_programCounter += 2;
            break;
        case 0x6000:
            // LD Vx, byte
            m_registers[x] = nn;
            m_programCounter += 2;
            break;
        case 0x7000:
            // ADD Vx, byte
            m_registers[x] += nn;
            m_programCounter += 2;
            break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000:
                    // LD Vx, Vy
                    m_registers[x] = m_registers[y];
                    m_programCounter += 2;
                    break;
                case 0x0001:
                    // OR Vx, Vy
                    m_registers[x] |= m_registers[y];
                    m_programCounter += 2;
                    break;
                case 0x0002:
                    // AND Vx, Vy
                    m_registers[x] &= m_registers[y];
                    m_programCounter += 2;
                    break;
                case 0x0003:
                    // XOR Vx, Vy
                    m_registers[x] ^= m_registers[y];
                    m_programCounter += 2;
                    break;
                case 0x0004:
                    // ADD Vx, Vy
                    {
                        uint16_t r = m_registers[x] + m_registers[y];

                        m_registers[0xF] = r > 0xFF ? 1 : 0;

                        m_registers[x] = static_cast<uint8_t>(r & 0xFF);

                        m_programCounter += 2;
                    }
                    break;
                case 0x0005:
                    // SUB Vx, Vy
                    m_registers[0xF] = m_registers[x] > m_registers[y] ? 1 : 0;
                    m_registers[x] -= m_registers[y];
                    m_programCounter += 2;
                    break;
                case 0x0006:
                    // SHR Vx {, Vy}
                    m_registers[0xF] = m_registers[x] & 0x1;
                    m_registers[x] >>= 1;
                    m_programCounter += 2;
                    break;
                case 0x0007:
                    // SUBN Vx, Vy
                    m_registers[0xF] = m_registers[y] > m_registers[x] ? 1 : 0;
                    m_registers[x] = m_registers[y] - m_registers[x];
                    m_programCounter += 2;
                    break;
                case 0x000E:
                    // SHL Vx, Vy
                    m_registers[0xF] = (m_registers[x] & 128) >> 7;
                    m_registers[x] <<= 1;
                    m_programCounter += 2;
                    break;
                default:
                    throw std::runtime_error(std::format("[ERROR] Unknown Op -> {:#06x}", opcode));
                    break;
            }
            break;
        case 0x9000:
            // SNE Vx, Vy
            if (m_registers[x] != m_registers[y]) m_programCounter += 4;
            else m_programCounter += 2;
            break;
        case 0xA000:
            // LD I, addr
            m_indexRegister = nnn;
            m_programCounter += 2;
            break;
        case 0xB000:
            // JP V0, addr
            m_programCounter = nnn + m_registers[0x0];
            break;
        case 0xC000:
            // RND Vx, byte
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<unsigned int> distr(0x0, 0xFF);
                uint8_t randNum = distr(gen);

                m_registers[x] = randNum & nn;
                m_programCounter += 2;
            }
            break;
        case 0xD000:
            // DRW Vx, Vy, nibble
            {
                uint8_t originX = m_registers[x];
                uint8_t originY = m_registers[y];

                m_registers[0xF] = 0;
                for (uint8_t yCoord = 0; yCoord < n; yCoord++) {
                    uint8_t pixel = memory.Read(yCoord + m_indexRegister);
                    for (uint8_t xCoord = 0; xCoord < 8; xCoord++) {
                        if ((pixel & (0x80 >> xCoord)) != 0) {
                            uint8_t pixelX = (originX + xCoord) % CHIP8_SCREEN_WIDTH;
                            uint8_t pixelY = (originY + yCoord) % CHIP8_SCREEN_HEIGHT;

                            // XOR display, ON -> OFF + Collision, OFF -> ON.
                            if (renderEngine.IsThisPixelOnAt(pixelX, pixelY)) {
                                renderEngine.ChangePixelStateAt(pixelX, pixelY, PixelState::PIXEL_OFF);
                                m_registers[0xF] = 1; // Collision detected.
                            } else {
                                renderEngine.ChangePixelStateAt(pixelX, pixelY, PixelState::PIXEL_ON);
                            }
                        }
                    }
                }

                m_programCounter += 2;
            }
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E:
                    // SKP Vx
                    if (m_keys[m_registers[x]] == KeyState::PRESSED) m_programCounter += 4;
                    else m_programCounter += 2;
                    break;
                case 0x00A1:
                    // SKNP Vx
                    if (m_keys[m_registers[x]] == KeyState::NOT_PRESSED) m_programCounter += 4;
                    else m_programCounter += 2;
                    break;
                default:
                    throw std::runtime_error(std::format("[ERROR] Unknown Op -> {:#06x}", opcode));
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007:
                    // LD Vx, DT
                    m_registers[x] = m_delayTimer;
                    m_programCounter += 2;
                    break;
                case 0x000A:
                    // LD Vx, K
                    throw std::runtime_error("[ERROR] LD Vx, K isn't implemented yet.");
                    break;
                case 0x0015:
                    // LD DT, Vx
                    m_delayTimer = m_registers[x];
                    m_programCounter += 2;
                    break;
                case 0x0018:
                    // LD ST, Vx
                    m_soundTimer = m_registers[x];
                    m_programCounter += 2;
                    break;
                case 0x001E:
                    // ADD I, Vx
                    m_indexRegister += m_registers[x];
                    m_programCounter += 2;
                    break;
                case 0x0029:
                    // LD F, Vx
                    m_indexRegister = m_registers[x] * 5;
                    m_programCounter += 2;
                    break;
                case 0x0033:
                    // LD B, Vx
                    {
                        uint8_t regVal = m_registers[x];

                        memory.Write(m_indexRegister, regVal / 100); // Hundreds.
                        memory.Write(m_indexRegister + 1, (regVal % 100) / 10); // Tens.
                        memory.Write(m_indexRegister + 2, regVal % 10); // Units.

                        m_programCounter += 2;
                    }
                    break;
                case 0x0055:
                    // LD [I], Vx
                    for (uint8_t i = 0; i < x + 1; i++) {
                        memory.Write(i + m_indexRegister, m_registers[i]);
                    }

                    m_programCounter += 2;
                    break;
                case 0x0065:
                    // LD Vx, [I]
                    for (uint8_t i = 0; i < x; i++) {
                        m_registers[i] = memory.Read(i + m_indexRegister);
                    }

                    m_programCounter += 2;
                    break;
                default:
                    throw std::runtime_error(std::format("[ERROR] Unknown Op -> {:#06x}", opcode));
                    break;
            }
            break;
        default:
            throw std::runtime_error(std::format("[ERROR] Unknown Op -> {:#06x}", opcode));
            break;
    }
}
