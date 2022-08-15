#include "cpu.hpp"

void CPU::LoadRom(const char* const filename)
{
    ifstream file(filename,std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        cout << "ERROR: Unable to open file\n";
        exit(1);
    }
    else //THIS ELSE STATEMENT IS FROM AUSTIN MORLAN, https://austinmorlan.com/posts/chip8_emulator/
    {
        // Get size of file and allocate a buffer to hold the contents
        std::streampos size = file.tellg();
        char* buffer = new char[size];
        // Go back to the beginning of the file and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        // Load the ROM contents into the Chip8's memory, starting at 0x200
        for (long i = 0; i < size; ++i)
        {
            memory[0x200 + i] = buffer[i];
        }
        // Free the buffer
        delete[] buffer;
    }
}

void CPU::cycle()
{
    opcode = (memory[pc]<<8|memory[pc+1]);
    pc+=2;
    switch ((opcode&0xF000)>>12){
        case 1:
            OP_1NNN();
            break;
        case 2:
            OP_2NNN();
            break;
        case 3:
            OP_3XNN();
            break;
        case 4:
            OP_4XNN();
            break;
        case 5:
            OP_5XY0();
            break;
        case 6:
            OP_6XKK();
            break;
        case 7:
            OP_7XKK();
            break;
        case 9:
            OP_9XY0();
            break;
        case 0xA:
            OP_ANNN();
            break;
        case 0xB:
            OP_BNNN();
            break;
        case 0xC:
            OP_CXKK();
            break;
        case 0xD:
            OP_DXYN();
            break;
        case 0xE:
            (opcode&0x000F)==1?OP_EXA1():OP_EX9E();
            break;
        case 0:
            (opcode&0x000F)==0?OP_00E0():OP_00EE();
            break;
        case 8:
            switch(opcode&0x000F)
            {
                case 0:
                    OP_8XY0();
                    break;
                case 1:
                    OP_8XY1();
                    break;
                case 2:
                    OP_8XY2();
                    break;
                case 3:
                    OP_8XY3();
                    break;
                case 4:
                    OP_8XY4();
                    break;
                case 5:
                    OP_8XY5();
                    break;
                case 7:
                    OP_8XY7();
                    break;
                case 8:
                    OP_8XY8();
                    break;
            }
            break;
        case 0xF:
            switch (opcode&0x000F)
            {
                case 7:
                    OP_FX07();
                    break;
                case 0xA:
                    OP_FX0A();
                    break;
                case 0x5:
                    switch (opcode&0x00F0)
                    {
                        case 0x10:
                            OP_FX15();
                            break;
                        case 0x50:
                            OP_FX55();
                            break;
                        case 0x60:
                            OP_FX65();
                            break;
                    }
                case 0x8:
                    OP_FX18();
                    break;
                case 0xE:
                    OP_FXIE();
                    break;
                case 0x9:
                    OP_FX29();
                case 0x3:
                    OP_FX33();
                    break;
            }
    }
    
}
