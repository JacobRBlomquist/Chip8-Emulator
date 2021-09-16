#include <SDL.h>
#include <chrono>
#include "Platform.hpp"

Platform::Platform(int pWidth, int pInstructionsPerSecond, Chip8 *pChip8Object) : gWidth(pWidth),
                                                                                  gHeight(gWidth / 2),
                                                                                  gChip8Object(pChip8Object),
                                                                                  gInstructionsPerSecond(pInstructionsPerSecond),
                                                                                  gWindow(nullptr),
                                                                                  gRenderer(nullptr),
                                                                                  gTexture(nullptr)
{
}

Platform::~Platform()
{
    SDL_DestroyTexture(gTexture);
    SDL_DestroyRenderer(gRenderer);

    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}

void Platform::Loop()
{
    bool running = true;
    SDL_Event event;
    uint32_t pixels[64 * 32];
    uint64_t lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    double unprocessedSeconds = 0;
    double secondsPerTick = 1.0 / (double)gInstructionsPerSecond;

    while (running)
    {
        // Events
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;

                for (int i = 0; i < 16; i++)
                {
                    if (event.key.keysym.sym == gChip8KeyMap[i])
                    {
                        gChip8Object->SetKeyState(i, 1);
                    }
                }
                break;
            case SDL_KEYUP:
                for (int i = 0; i < 16; i++)
                {
                    if (event.key.keysym.sym == gChip8KeyMap[i])
                    {
                        gChip8Object->SetKeyState(i, 0);
                    }
                }
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    printf("Window Resized: %d X %d\n", event.window.data1, event.window.data2);
                    fflush(stdout);
                }
                break;
            }
        }

        //Cycle
        uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        unprocessedSeconds += ((double)(currentTime - lastTime) / 1000.0);
        lastTime = currentTime;

        while (unprocessedSeconds >= secondsPerTick)
        {
            gChip8Object->Cycle();
            unprocessedSeconds -= secondsPerTick;
        }

        //Render
        const uint8_t *chip8Pixels = gChip8Object->GetScreen();
        for (int i = 0; i < 64 * 32; i++)
        {
            uint8_t curPixel = chip8Pixels[i];
            pixels[i] = (0x00FFFFFF * curPixel) | 0xFF000000;
        }

        SDL_UpdateTexture(gTexture, NULL, pixels, 64 * sizeof(uint32_t));
        SDL_RenderClear(gRenderer);
        SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
        SDL_RenderPresent(gRenderer);
    }
}

int Platform::InitPlatform(const char *pTitle)
{
    int status = SDL_Init(SDL_INIT_VIDEO);
    if (status != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    gWindow = SDL_CreateWindow(pTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWidth, gHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (gWindow == nullptr)
    {
        printf("Failed to create window. ERROR: %s\n", SDL_GetError());
        return 1;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == nullptr)
    {
        printf("Failed to create renderer. ERROR: %s\n", SDL_GetError());
        return 1;
    }

    gTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (gTexture == nullptr)
    {
        printf("Failed to create texture. ERROR: %s\n", SDL_GetError());
        return 1;
    }
    return 0;
}
