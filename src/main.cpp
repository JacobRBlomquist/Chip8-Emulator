/**
 * Copyright 2021 - Jacob R. Blomquist
 * Original Author: Jacob R. Blomquist <BlomDev at gmail dot com>
 * License: MIT
 */

#include <cstdio>
#include <SDL.h>
#include <fstream>
#include "Chip8.hpp"
#include "Platform.hpp"


int LoadRomFile(Chip8 &pChip8, const char *pFileName)
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

	uint8_t *buffer = new uint8_t[fileSize];

	inFile.read((char *)buffer, fileSize);
	inFile.close();

	bool ret = pChip8.LoadRom(buffer, (uint32_t)fileSize);

	delete[] buffer;

	return ret;
}


int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("usage: %s RomFile\n", argv[0]);
		return 1;
	}

	const char *romFile = argv[1];

	Chip8 chip8;
	if (LoadRomFile(chip8, romFile) != 0)
	{
		return 1;
	}

	Platform platform(800, 700, &chip8);
	if (platform.InitPlatform("WIndow Title") != 0)
	{
		return 1;
	}

	platform.Loop();

	return 0;
}
