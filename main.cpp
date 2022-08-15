//
//  main.cpp
//  CHIP-8
//
//  Created by Alex on 4/22/22.
//

#include <iostream>
#include <stack>
#include "cpu.hpp"
#include "platform.hpp"
#include <getopt.h>
using namespace std;

struct option long_options[] =
    {
        {"help",          no_argument,       NULL,       'h'},
        {"scale",         required_argument, NULL, 's'},
        {"delay",         required_argument, NULL, 'd'},
        {"rom",           required_argument, NULL, 'r'},
        {nullptr,      0,                nullptr,  0}
    };

int main(int argc,char * argv[]) {
    int choice;
    int size = 10;
    int delay = 3;
    string rom = "NA";
    while ((choice = getopt_long(argc, argv, "hs:d:r:", long_options,
                                 nullptr)) != -1)
    {
        switch(choice){
            case 'h':
                cout << "Usage:\n";
                cout << "Uses 3 arguments, the scale, the delay, and the rom (required) to load\n";
                cout << "Scale: -s or --scale with an int is the size of the window. 1 is 64x32, 2 is 128x64, etc. Default is 10\n";
                cout << "Delay: -d or --delay with an int is how fast the emulation goes, more being slower. 1 is far too fast for most modern pcs and is unplayable, but can be a fun challenge. 3 is default\n";
                cout << "Rom:   -r or --rom. Should be obvious, just the path to the rom file\n";
                exit(0);
                break;
            case 's':
                size = atoi(optarg);
                break;
            case 'd':
                delay = atoi(optarg);
                break;
            case'r':
                rom = optarg;
                break;
        }
    }
    if (rom == "NA")
    {
        cout << "Please specify a ROM path (also if the rom path is NA (if this is you, please reconsider your naming conventions) it will not work, sorry (but not really, please name your files something better))\nRun with -h or --help for help if needed\n";
    }
    Platform platform("CHIP-8", 64*size, 32*size, 64, 32);
    CPU cpu;
    const char* temp = rom.c_str();
    cpu.LoadRom(temp);
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;
    while (!quit)
    {
        quit = platform.ProcessInput(cpu.keypad);
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime-lastCycleTime).count();
        
        if (dt > delay)
        {
            lastCycleTime = currentTime;
            for (int i = 0; i < 8; i++)
            {
                cpu.cycle();
            }
            cpu.timer>0?cpu.timer--:cpu.timer=0;
            cpu.soundTimer>0?cpu.soundTimer--:cpu.soundTimer=0;
            platform.Update(cpu.video, size);
            
        }
    }
    
    return 0;
}
