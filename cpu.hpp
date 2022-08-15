//
//  cpu.cpp
//  CHIP-8
//
//  Created by Alex on 7/27/22.
//
#include <fstream>
#include <chrono>
#include <random>
#include <iostream>
using namespace std;
class CPU{
public:
    default_random_engine randGen;
    uniform_int_distribution<uint8_t> randByte;
    CPU(): randGen(chrono::system_clock::now().time_since_epoch().count())
    {
        for (int i = 0; i < 80; i++)
        {
            memory[0x50+i] = fontset[i];
        }
        randByte = uniform_int_distribution<uint8_t>(0, 255U);
        for (int i = 0; i < 16; i++)
        {
            registers[i] = 0;
            keypad[i] = 0;
        }
        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                video[i][j] = 0;
            }
        }
    }
    uint8_t registers[16]; //Reg VF (the last one) is flag)
    uint8_t memory[4096]; //0x000-0x1FF is (usually) ignorable, 0x050-0x0A0 is for special characters, 0x200-rest is the rom itself
    uint16_t index; //Used to store addresses as an 8bit is too small to do that
    uint16_t pc = 0x200; //Program counter
    stack<uint16_t> callStack; //Stack (obviously)
    uint8_t timer; //Used for something, not sure yet though
    uint8_t soundTimer;//See above
    uint8_t keypad[16];//Used for input
    uint32_t video[64][32];//Video is an 2d array on or off
    uint16_t opcode;//Current instruction
    uint8_t fontset[80] = //THESE FONTS ARE ALSO FROM AUSTIN MORLAN
    {
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
    void LoadRom(const char* const filename);
    void OP_00E0() //Clear Display
    {
        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                video[i][j] = 0;
            }
        }
    }
    void OP_00EE() //Return from subroutine
    {
        pc = callStack.top();
        callStack.pop();
    }
    void OP_1NNN() //Jump
    {
        pc = opcode & 0x0FFF;
    }
    void OP_2NNN() //Call subroutine
    {
        callStack.push(pc);
        pc = opcode & 0x0FFF;
    }
    void OP_3XNN() //Skip next inst if reg x == nn
    {
        registers[(opcode&0x0F00)>>8] == (opcode&0x00FF) ? pc += 2 : pc += 0;
    }
    void OP_4XNN() //Skip next inst if reg x != nn
    {
        registers[(opcode&0x0F00)>>8] != (opcode&0x00FF) ? pc += 2 : pc += 0;
    }
    void OP_5XY0() //Skip next inst if reg x == reg y
    {
        registers[(opcode&0x0F00)>>8] == (registers[(opcode&0x00F0)>>4]) ? pc += 2: pc += 0;
    }
    void OP_6XKK() //Load kk into reg x
    {
        registers[(opcode&0x0F00)>>8] = opcode&0x00FF;
    }
    void OP_7XKK() //Reg x = reg x + kk
    {
        registers[(opcode&0x0F00)>>8] += opcode&0x00FF;
    }
    void OP_8XY0() //Reg x = reg y
    {
        registers[(opcode&0x0F00)>>8] = registers[(opcode&0x00F0)>>4];
    }
    void OP_8XY1() //Reg x = reg x | regy
    {
        registers[(opcode&0x0F00)>>8] |= registers[(opcode&0x00F0)>>4];
    }
    void OP_8XY2() //Reg x = reg x & regy
    {
        registers[(opcode&0x0F00)>>8] &= registers[(opcode&0x00F0)>>4];
    }
    void OP_8XY3() //Reg x = reg x ^ regy
    {
        registers[(opcode&0x0F00)>>8] ^= registers[(opcode&0x00F0)>>4];
    }
    void OP_8XY4() //Reg x = reg x + regy and set flag
    {
        registers[(opcode&0x0F00)>>8] + registers[(opcode&0x00F0)>>4] > 255 ? registers[15] = 1 : registers[15] = 0;
        registers[(opcode&0x0F00)>>8] = (registers[(opcode&0x0F00)>>8] + registers[(opcode&0x00F0)>>4]) & 0xFF;
    }
    void OP_8XY5() //Reg x = reg x - reg y and set flag
    {
        registers[(opcode&0x0F00)>>8] > registers[(opcode&0x00F0)>>4] ? registers[15] = 1 : registers[15] = 0;
        registers[(opcode&0x0F00)>>8] -= registers[(opcode&0x00F0)>>4];
    }
    void OP8XY6() //Shift x right 1, save the LSB in flag
    {
        registers[15] = registers[(opcode&0x0F00)>>8] &0x1;
        registers[(opcode&0x0F00)>>8] >>= 1;
    }
    void OP_8XY7() //Reg x = reg y - reg x and set flag
    {
        registers[(opcode&0x0F00)>>8] < registers[(opcode&0x00F0)>>4] ? registers[15] = 1 : registers[15] = 0;
        registers[(opcode&0x0F00)>>8] = registers[(opcode&0x00F0)>>4] - registers[(opcode&0x0F00)>>8];
    }
    void OP_8XY8() //Shift x left 1, save the LSB in flag
    {
        registers[15] = registers[(opcode&0x0F00)>>8] &0x1;
        registers[(opcode&0x0F00)>>8] <<= 1;
    }
    void OP_9XY0() //Skip next inst if x != y
    {
        registers[(opcode&0x0F00)>>8] != registers[(opcode&0x00F0)>>4] ? pc += 2 : pc += 0;
    }
    void OP_ANNN() //Set index to NNN
    {
        index = opcode&0x0FFF;
    }
    void OP_BNNN() //Jump to reg 0 + NNN
    {
        pc = registers[0] + (opcode&0x0FFF);
    }
    void OP_CXKK() //Set reg x to a random number & KK
    {
        registers[(opcode&0x0F00)>>8] = randByte(randGen) & (opcode & 0x00FF);
    }
    void OP_DXYN() //Display sprite
    {
        uint8_t x = registers[((opcode&0x0F00)>>8)]%64; //Modulo is for wraparound
        uint8_t y = registers[((opcode&0x00F0)>>4)]%32;
        uint8_t n = opcode&0x000F;
        registers[15] = 0;
        for (int i = index; i < n+index; i++)
        {
            uint8_t spriteByte = memory[i];
            for (int j = 0; j < 8; j++)
            {
                uint8_t spritePixel = spriteByte & (0x80 >> j);
                uint32_t* screenPixel = &video[x + j][y + i-index];
                if (spritePixel)
                {
                    if (*screenPixel == 0xFFFFFFFF)
                    {
                        registers[0xF] = 1; //Turn on collision
                    }
                    *screenPixel ^= 0xFFFFFFFF;
                }
            }
        }
    }
    void OP_EX9E() //Skip next inst if key corresponding to x register is pressed
    {
        keypad[registers[(opcode&0x0F00)>>8]]?pc+=2:pc+=0;
    }
    void OP_EXA1() //Opposite of the last one
    {
        !keypad[registers[(opcode&0x0F00)>>8]]?pc+=2:pc+=0;
    }
    void OP_FX07() //Set reg x to timer value
    {
        registers[(opcode&0x0F00)>>8] = timer;
    }
    void OP_FX0A() //Wait for a keypress, then store that into reg x
    {
        uint8_t reg = (opcode&0x0F00)>>8;
        if (keypad[0]) {registers[reg] = 0;}
        else if (keypad[1]) {registers[reg] = 1;}
        else if (keypad[2]) {registers[reg] = 2;}
        else if (keypad[3]) {registers[reg] = 3;}
        else if (keypad[4]) {registers[reg] = 4;}
        else if (keypad[5]) {registers[reg] = 5;}
        else if (keypad[6]) {registers[reg] = 6;}
        else if (keypad[7]) {registers[reg] = 7;}
        else if (keypad[8]) {registers[reg] = 8;}
        else if (keypad[9]) {registers[reg] = 9;}
        else if (keypad[10]) {registers[reg] = 10;}
        else if (keypad[11]) {registers[reg] = 11;}
        else if (keypad[12]) {registers[reg] = 12;}
        else if (keypad[13]) {registers[reg] = 13;}
        else if (keypad[14]) {registers[reg] = 14;}
        else if (keypad[15]) {registers[reg] = 15;}
        else {pc-=2;}
    }
    void OP_FX15() //Set timer to value in reg x
    {
        timer = registers[(opcode&0x0F00)>>8];
    }
    void OP_FX18() //Set sound timer to value in reg x
    {
        soundTimer = registers[(opcode&0x0F00)>>8];
    }
    void OP_FXIE() //Index = Index + reg x
    {
        index += registers[(opcode&0x0F00)>>8];
    }
    void OP_FX29() //Set index to the location of a specific sprite
    {
        index = 0x50 + (5*registers[(opcode&0x0F00)>>8]);
    }
    void OP_FX33() //Store BCD rep of x in memory @ index, index + 1, and index + 2
    {
        uint8_t temp = registers[(opcode&0x0F00)>>8];
        memory[index+2] = temp%10;
        temp /= 10;
        memory[index+1] = temp%10;
        temp /=10;
        memory[index] = temp%10;
    }
    void OP_FX55() //Store registers 0-x in memory @ index
    {
        for (int i = 0; i <= (opcode&0x0F00) >> 8; i++)
        {
            memory[i+index] = registers[i];
        }
    }
    void OP_FX65() //The opposite of the last one
    {
        for (int i = 0; i <= (opcode&0x0F00) >> 8; i++)
        {
            registers[i] = memory[i+index];
        }
    }
    void cycle();
};
