#pragma once
#include <cstdint>
#include <random>

class Chip8
{
private:
public:
    uint8_t memory[4096];
    uint32_t video[64 * 32];
    uint16_t pc{};
    uint16_t opcode{};
    uint16_t index{};
    uint16_t sp{};
    uint16_t stack[16];
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t registers[16];
    uint8_t keypad[16]{};
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
    Chip8();
    void LoadROM(char const* filename);
    void LoadFont();
    void Cycle();
    void OP_DXYN();
    
};