#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL.h>
#include "Chip8.hpp"

class Platform
{
public:
    Platform(int pWidth, int pInstructionsPerSecond, Chip8 * pChip8Object);
    virtual ~Platform();

public:
    // Event Loop for input and running Chip8
    void Loop();

    /*
    Initializes SDL
    Returns 1 if an error occurs and 0 otherwise. Will print error to stdout.
    */
    int InitPlatform(const char * pWindowTitle);




private:
    int gWidth;
    int gHeight;
    int gInstructionsPerSecond;

    Chip8 * gChip8Object;

    SDL_Window *gWindow;
    SDL_Renderer *gRenderer;
    SDL_Texture *gTexture;

    // Key Map for Chip8 (See Readme)
    SDL_KeyCode gChip8KeyMap[16] = {
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
    };


};

#endif // PLATFORM_H