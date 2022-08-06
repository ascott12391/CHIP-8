//
//  platform.cpp
//  CHIP-8
//
// NOT created by Alex on 8/2/22.
// created by Austin Morlan on some unknown date (6/8/22?).
//
// Copied (with very, very, very slight changes) from Austin Morlan again because I'm a hack and a fraud and still need to learn more SDL :D
// Also I'm writing this on my M1 Mac and SDL doesn't seem to play well with it sometimes :D


// :(

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

    void Update(void const* buffer, int pitch);

    bool ProcessInput(uint8_t* keys);

private:
    SDL_Window* window = NULL; //Literally changed these to init to NULL and thats it
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;
};
