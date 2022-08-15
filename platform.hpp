//
//  platform.cpp
//  CHIP-8
//
// NOT created by Alex on 8/2/22.
// created by Austin Morlan on some unknown date (6/8/22?).
//
// Copied in part from Austin Morlan again because I'm a hack and a fraud and still need to learn more SDL :D
// Also I'm writing this on my M1 Mac and SDL doesn't seem to play well with it sometimes :D


// :(

// Update: Had to change fundamentally how update works, so learned some SDL from that :)

#include <iostream>
#include <SDL2/SDL.h>
using namespace std;
class Platform
{
public:
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
    {
        SDL_Init(SDL_INIT_VIDEO);

        window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        texture = SDL_CreateTexture(
            renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
    }

    ~Platform()
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Update(const uint32_t (&video)[64][32], int pitch);

    bool ProcessInput(uint8_t* keys);
    
    SDL_Renderer* renderer = NULL;

private:
    SDL_Window* window = NULL; //Literally changed these to init to NULL and thats it
    SDL_Texture* texture = NULL;
};
