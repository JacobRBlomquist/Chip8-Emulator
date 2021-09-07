/**
 * Copyright 2021 - Jacob R. Blomquist
 * Original Author: Jacob R. Blomquist <BlomDev at gmail dot com>
 * License: MIT
 */

#include <cstdio>
#include <SDL.h>
#include <fstream>
#include <chrono>
#include "Chip8.hpp"


const int WIDTH = 800;
const int HEIGHT = WIDTH / 16 * 9;
const int INSTRUCTIONS_PER_SECOND = 700;

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* gTexture = nullptr;

SDL_KeyCode chip8KeyMap[16] = {
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

int SetupSDL(const char* pTitle, int pWidth, int pHeight)
{
	int status = SDL_Init(SDL_INIT_VIDEO);
	if (status != 0)
	{
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	gWindow = SDL_CreateWindow(pTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pWidth, pHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

bool InitChip8(Chip8& pChip8, const char* pFileName)
{
	// load ROM file 
	std::ifstream inFile(pFileName, std::ifstream::binary | std::ifstream::ate);
	if (!inFile.is_open())
	{
		printf("Could not open file: %s\n", pFileName);
		return 1;
	}

	std::streampos fileSize = inFile.tellg();
	inFile.seekg(0);

	uint8_t* buffer = new uint8_t[fileSize];

	inFile.read((char*)buffer, fileSize);
	inFile.close();

	bool ret = pChip8.LoadRom(buffer, (uint32_t)fileSize);
	
	delete[] buffer;

	return ret;
}

void MainLoop(Chip8& pChip8)
{
	bool running = true;
	SDL_Event event;
	uint32_t pixels[64 * 32];
	uint64_t lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	double unprocessedSeconds = 0;
	double secondsPerTick = 1.0 / (double)INSTRUCTIONS_PER_SECOND;

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

				for (int i = 0; i < 0x16; i++)
				{
					if (event.key.keysym.sym == chip8KeyMap[i])
					{
						pChip8.SetKeyState(i, 1);
					}
				}
				break;
			case SDL_KEYUP:
				for (int i = 0; i < 0x16; i++)
				{
					if (event.key.keysym.sym == chip8KeyMap[i])
					{
						pChip8.SetKeyState(i, 0);
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

		while (unprocessedSeconds >= secondsPerTick) {
			pChip8.Cycle();
			unprocessedSeconds -= secondsPerTick;

		}


		//Render
		const uint8_t* chip8Pixels = pChip8.GetScreen();
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

void DestroySDL()
{
	SDL_DestroyTexture(gTexture);
	SDL_DestroyRenderer(gRenderer);

	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("usage: %s RomFile\n", argv[0]);
		return 1;
	}

	const char* romFile = argv[1];

	if (SetupSDL("Chip8 Emulator - V-1.0", WIDTH, HEIGHT) != 0)
		return 1;

	Chip8 chip8;
	if (InitChip8(chip8, romFile) != 0)
	{
		return 1;
	}

	MainLoop(chip8);

	DestroySDL();

	return 0;
}
