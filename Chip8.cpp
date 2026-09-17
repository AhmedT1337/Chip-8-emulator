#include "Chip8.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <random>
#include <iostream>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

uint8_t FONTSET[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
void Chip8::LoadFont(){
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i){
        memory[FONTSET_START_ADDRESS + i] = FONTSET[i];
    }
}

void Chip8::LoadROM(char const* filename){
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streampos size = file.tellg();
    char* buffer = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();
    for (long i = 0; i<size; ++i){
        memory[START_ADDRESS + i] = buffer[i];
    }
    delete[] buffer;
}

Chip8::Chip8(): randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
    memset(video, 0, sizeof(video));
    LoadFont();
    pc = START_ADDRESS;
}

void Chip8::Cycle(){
    opcode = memory[pc] << 8 | memory[pc+1];
    pc += 2;
    switch (opcode & 0xF000)
    {
    case 0x0000:
        /* code */
        
        if ((opcode & 0x00FF) == 0x00E0){
            memset(video, 0, sizeof(video));
        } else if ((opcode & 0x00FF) == 0x00EE){
            if (sp > 0){
                --sp;
                pc = stack[sp];
            }
        }
        break;
    case (0x1000):
        {
        uint16_t address = opcode & 0x0FFF;
        pc = address;
        }
        break;
    case (0x2000):
        {
            uint16_t address = opcode & 0x0FFF;
            if (sp < 16){
                stack[sp] = pc;
                ++sp;
                pc = address;
            }
        }    
        break;
    case (0x3000):
        {
            uint8_t Vx = (opcode & 0x0F00) >> 8;
            uint8_t kk = opcode & 0x00FF;
            if (registers[Vx] == kk){
                pc += 2;
            }
        }    
        break;
    case (0x4000):
        {
            uint8_t Vx = (opcode & 0x0F00) >> 8;
            uint8_t kk = opcode & 0x00FF;
            if (registers[Vx] != kk){
                pc += 2;
            }
        }    
        break;
    case (0x5000):
        {
        uint8_t Vx = (opcode & 0x0F00) >> 8;
        uint8_t Vy = (opcode & 0x00F0) >> 4;
        if (registers[Vx] == registers[Vy]){
            pc+=2;
        }
        }
        break;
    case (0x6000):
        {
            uint8_t Vx = (opcode & 0x0F00) >> 8;
            uint8_t NN = opcode & 0x00FF;
            registers[Vx] = NN;
        }
        break;
    case (0x7000):
        {    
        uint8_t Vx = (opcode & 0x0F00) >> 8;
        uint8_t NN = opcode & 0x00FF;
        registers[Vx] += NN;        
        }    
        break;
    case (0x8000):
    {
        uint8_t Vx = (opcode & 0x0F00) >> 8;
        uint8_t Vy = (opcode & 0x00F0) >> 4;
        uint8_t op = opcode & 0x000F;
        if (op == 0x0000){
            registers[Vx] = registers[Vy];
        }
        else if (op == 1){
            registers[Vx] |= registers[Vy];
        }
        else if (op == 2){
            registers[Vx] &= registers[Vy];
        }
        else if (op == 3){
            registers[Vx] ^= registers[Vy];
        }
        else if (op == 4){
            uint16_t sum = registers[Vx] + registers[Vy];
            registers[0xF] = (sum > 255) ? 1 : 0;
            registers[Vx] = sum & 0xFF;
            
        }
        else if (op == 5){
            registers[0xF] = (registers[Vx] > registers[Vy]) ? 1 : 0;
            registers[Vx] -= registers[Vy];
        }
        else if (op == 6){
            registers[Vx] = registers[Vy];
            registers[0xF] = registers[Vx] & 0x1;
            registers[Vx] >>=1;
        }
        else if (op == 0xE){
            registers[Vx] = registers[Vy];
            registers[0xF] = (registers[Vx] & 0x80) >> 7;
            registers[Vx] <<=1;
        }
        else if (op == 7){
            registers[0xF] = (registers[Vy] > registers[Vx]) ? 1 : 0;
            registers[Vx] = registers[Vy] - registers[Vx];
        }
    }        
        break;
    case (0x9000):
        {
        uint8_t Vx = (opcode & 0x0F00) >> 8;
        uint8_t Vy = (opcode & 0x00F0) >> 4;
        if (registers[Vx] != registers[Vy]){
            pc+=2;
        }
        }
        break;
        
    case (0xA000):
        {
            uint16_t NNN = opcode & 0x0FFF;
            index = NNN;
        }
        break;
    
    case (0xB000):
        {
            uint16_t address = opcode & 0x0FFF;
            pc = address + registers[0];
        }
        break;
    
    case (0xC000):
        {uint8_t Vx = (opcode & 0x0F00) >> 8;
        uint8_t kk = opcode & 0x00FF;
        registers[Vx] = randByte(randGen) & kk;}
        break;
    
    case (0xD000):
        OP_DXYN();
        break;
    
    case(0xE000):
        {
            uint8_t op = (opcode & 0x000F);
            if (op == 0xE){
                uint8_t Vx = (opcode & 0x0F00u) >> 8;
                uint8_t key = registers[Vx];
                if (keypad[key])
                {
                    pc += 2;
                }
            } else if (op == 0x1){
                uint8_t Vx = (opcode & 0x0F00u) >> 8;
                uint8_t key = registers[Vx];
                if (!keypad[key])
                {
                    pc += 2;
                }
            }
        }
        break;
        
    case (0xF000):
    {
        uint8_t op = opcode & 0x00FF;
        uint8_t Vx = (opcode & 0x0F00) >> 8;
        if (op == 0x07){
            registers[Vx] = delayTimer;
        } else if (op == 0x0A){
            if (keypad[0])
            {
                registers[Vx] = 0;
            }
            else if (keypad[1])
            {
                registers[Vx] = 1;
            }
            else if (keypad[2])
            {
                registers[Vx] = 2;
            }
            else if (keypad[3])
            {
                registers[Vx] = 3;
            }
            else if (keypad[4])
            {
                registers[Vx] = 4;
            }
            else if (keypad[5])
            {
                registers[Vx] = 5;
            }
            else if (keypad[6])
            {
                registers[Vx] = 6;
            }
            else if (keypad[7])
            {
                registers[Vx] = 7;
            }
            else if (keypad[8])
            {
                registers[Vx] = 8;
            }
            else if (keypad[9])
            {
                registers[Vx] = 9;
            }
            else if (keypad[10])
            {
                registers[Vx] = 10;
            }
            else if (keypad[11])
            {
                registers[Vx] = 11;
            }
            else if (keypad[12])
            {
                registers[Vx] = 12;
            }
            else if (keypad[13])
            {
                registers[Vx] = 13;
            }
            else if (keypad[14])
            {
                registers[Vx] = 14;
            }
            else if (keypad[15])
            {
                registers[Vx] = 15;
            }
            else
            {
                pc -= 2;
            }
        } else if (op == 0x15){
            delayTimer = registers[Vx];
        } else if (op == 0x18){
            soundTimer = registers[Vx];
        } else if (op == 0x1E){
            index += registers[Vx];
        } else if (op == 0x29){
            index = FONTSET_START_ADDRESS + (5 * registers[Vx]);
        } else if (op == 0x33){
            uint8_t value = registers[Vx];
            memory[index + 2] = value % 10;
            value /= 10;
            memory[index + 1] = value % 10;
            value /= 10;
            memory[index] = value % 10;
            
        } else if (op == 0x55){
            for (uint8_t i = 0; i <= Vx; ++i){
                memory[i + index] = registers[i];
            }
        } else if (op == 0x65){
            for (uint8_t i = 0; i <= Vx; ++i){
                registers[i] = memory[index + i];
            }
        } 
    }
    break;
    default:
        std::cout << "unknown instruction";
        break;
    }
    
    if (pc < 0x200 || pc >= 0xFFF) return;
    
    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) --soundTimer;
}

void Chip8::OP_DXYN(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    uint8_t height = opcode & 0x000F;
    uint8_t xPos = registers[Vx] % 64;
    uint8_t yPos = registers[Vy] % 32;
    registers[0xF] = 0;
    for (unsigned int row = 0; row < height; ++row){
        if ((row + index) >= 4096) break;
        if ((yPos + row) >= 32) break;
        uint8_t spriteByte = memory[index+row];
        for (unsigned int col = 0; col < 8; ++col){
            uint8_t spritePixel = spriteByte & (0x80 >> col);
            if (spritePixel){
                if ((xPos + col) >= 64) break;
                unsigned int screenIndex = (yPos + row) * 64 + (xPos + col);
                if (video[screenIndex] == 0xFFFFFFFF){
                    registers[0xF] = 1;
                }
                video[screenIndex] ^= 0xFFFFFFFF;
            }
        }
    }
}