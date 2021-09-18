/**
 * Copyright 2021 - Jacob R. Blomquist
 * Original Author: Jacob R. Blomquist <BlomDev at gmail dot com>
 * License: MIT
 */

#include "Chip8.hpp"
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <cstdio>

Chip8::Chip8()
{
    //reset processor state
    ResetState();
}
Chip8::~Chip8()
{
}

void Chip8::ResetState()
{
    // get current time in milliseconds since unix epoch
    lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    sp = 0;
    for (int i = 0; i < 16; i++)
    {
        V[i] = 0;
        keys[i] = 0;
    }

    //reset memory
    memset(memory, 0,0x1000);

    for (int i = 0; i < 64 * 32; i++)
        display[i] = 0;

    PC = 0x200;
    I = 0;
    delay = 0;
    sound = 0;
}

void Chip8::Cycle()
{
    uint16_t opcode = Fetch();
    DecodeAndExecute(opcode);

    //timers
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    unprocessedTime += ((double)(currentTime - lastTime) / 1000.0); 
    lastTime = currentTime;

    while (unprocessedTime >= secondsPer60Hz)
    {
        if (delay > 0)
            delay--;
        if (sound > 0)
            sound--;

        unprocessedTime -= secondsPer60Hz;
    }
}

uint16_t Chip8::Fetch()
{
    uint16_t ret = 0;
    ret = memory[PC] << 8 | memory[PC + 1];
    PC += 2;
    return ret;
}

void Chip8::DecodeAndExecute(uint16_t pOpcode)
{
    uint8_t highNibble = (pOpcode & 0xF000) >> 12;

    // index into function table on the high nibble and call the respective function
    (this->*(FunctionTable[highNibble]))(pOpcode);
}

void Chip8::SetKeyState(uint8_t keyCode, uint8_t state)
{
    if (keyCode > 0xF)
        return;

    keys[keyCode] = state;
}

const uint8_t *Chip8::GetScreen()
{
    return display;
}

bool Chip8::LoadRom(uint8_t *romData, uint32_t romSize)
{
    //reset state
    ResetState();

    //load ROM into memory
    if (romSize >= (0x1000 - 0x200))
    {
        printf("Rom Is Too Large!\n");
        return 1;
    }
    std::memcpy(&memory[0x200], romData, romSize);
    //load FONT
    std::memcpy(&memory[0x50], &font, 80);

    return 0;
}

void Chip8::zeroGroup(uint16_t opcode)
{
    uint8_t nibble = opcode & 0x000F;
    if (nibble == 0)
    {
        cls00E0(opcode);
    }
    else if (nibble == 0x000E)
    {
        ret00EE(opcode);
    }
    else
    {
        printf("Unknown ZeroGroup Opcode: %04x\n", opcode);
    }
}

void Chip8::cls00E0(uint16_t opcode)
{
    for (int i = 0; i < 64 * 32; i++)
        display[i] = 0;
}
void Chip8::ret00EE(uint16_t opcode)
{
    if (sp == 0)
    {
        printf("ERROR: Tried to RET when at end of call stack!\n");
        return;
    }

    sp--;

    PC = stack[sp];
}
void Chip8::jmp1NNN(uint16_t opcode)
{
    uint16_t addr = opcode & 0x0FFF;
    PC = addr;
}
void Chip8::call2NNN(uint16_t opcode)
{
    stack[sp] = PC;
    sp++;
    uint16_t addr = opcode & 0x0FFF;
    PC = addr;
}
void Chip8::skip3XNN(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;

    if (V[x] == val)
        PC += 2;
}
void Chip8::skip4XNN(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;

    if (V[x] != val)
        PC += 2;
}
void Chip8::skip5XY0(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if (V[x] == V[y])
        PC += 2;
}
void Chip8::set6XNN(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;

    V[x] = val;
}
void Chip8::add7XNN(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;
    V[x] += val;
}

void Chip8::eightGroup(uint16_t opcode)
{
    uint8_t nibble = opcode & 0x000F;
    switch (nibble)
    {
    case 0:
        setxy8XY0(opcode);
        break;
    case 1:
        or8XY1(opcode);
        break;
    case 2:
        and8XY2(opcode);
        break;
    case 3:
        xor8XY3(opcode);
        break;
    case 4:
        add8XY4(opcode);
        break;
    case 5:
        sub8XY5(opcode);
        break;
    case 6:
        shr8XY6(opcode);
        break;
    case 7:
        sub28XY7(opcode);
        break;
    case 0xE:
        shl8XYE(opcode);
        break;
    default:
        printf("Unkown 8 group opcode %04x\n", opcode);
    }
}
void Chip8::setxy8XY0(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    V[x] = V[y];
}
void Chip8::or8XY1(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] = V[x] | V[y];
}
void Chip8::and8XY2(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] = V[x] & V[y];
}
void Chip8::xor8XY3(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] = V[x] ^ V[y];
}
void Chip8::add8XY4(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[0xF] = 0;

    uint16_t sum = V[x] + V[y];
    if (sum > 255)
        V[0xF] = 1;

    V[x] = sum & 0xFF;
}
void Chip8::sub8XY5(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[0xF] = 0;
    if (V[x] > V[y])
        V[0xF] = 1;

    V[x] = V[x] - V[y];
}
void Chip8::sub28XY7(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[0xF] = 0;
    if (V[y] > V[x])
        V[0xF] = 1;

    V[x] = V[y] - V[x];
}
void Chip8::shr8XY6(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    V[0xF] = 0;
    if (V[x] & 1)
        V[0xF] = 1;

    V[x] = V[x] >> 1;
}
void Chip8::shl8XYE(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    V[0xF] = 0;
    if (V[x] & 0x80)
        V[0xF] = 1;

    V[x] = V[x] << 1;
}
void Chip8::skip9XY0(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if (V[x] != V[y])
    {
        PC += 2;
    }
}
void Chip8::setIANNN(uint16_t opcode)
{
    uint16_t val = opcode & 0x0FFF;
    I = val;
}
void Chip8::jmpoffBNNN(uint16_t opcode)
{
    uint16_t val = opcode & 0x0FFF;
    PC = val + V[0];
}
void Chip8::randCXNN(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t mask = opcode & 0x00FF;

    V[x] = rand() & mask;
}
void Chip8::drawDXYN(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t N = opcode & 0x000F;

    uint8_t xPos = (V[x]) % 64;
    uint8_t yPos = (V[y]) % 32;

    V[0xF] = 0; //clear flag bit

    for (int i = 0; i < N; i++)
    {
        uint8_t curLineByte = memory[I + i];
        for (int bit = 0; bit < 8; bit++)
        {
            int xPix = (xPos + bit) % 64;
            int yPix = (yPos + i) % 32;

            //pixel is ON
            if (curLineByte & (0x80) >> bit)
            {
                //if already on turn off and set flag bit
                if (display[xPix + yPix * 64])
                {
                    display[xPix + yPix * 64] = 0;
                    V[0xF] = 1;
                }
                else
                {
                    display[xPix + yPix * 64] = 1;
                }
            }
        }
    }
}
void Chip8::egroup(uint16_t opcode)
{
    uint8_t lowByte = opcode & 0x00FF;
    if (lowByte == 0x009E)
    {
        skipkeyEX9E(opcode);
    }
    else if (lowByte == 0x00A1)
    {
        skipnkeyEXA1(opcode);
    }
    else
    {
        printf("Unknown E group opcode %04x\n", opcode);
    }
}
void Chip8::skipkeyEX9E(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    if (keys[V[x]])
        PC += 2;
}
void Chip8::skipnkeyEXA1(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    if (!keys[V[x]])
        PC += 2;
}
void Chip8::fgroup(uint16_t opcode)
{
    uint8_t lowByte = opcode & 0x00FF;
    switch (lowByte)
    {
    case 0x07:
        gettimerFX07(opcode);
        break;
    case 0x15:
        settimerFX15(opcode);
        break;
    case 0x18:
        setsoundFX18(opcode);
        break;
    case 0x1e:
        addIFX1E(opcode);
        break;
    case 0x0a:
        waitinputFX0A(opcode);
        break;
    case 0x29:
        getFontCharFX29(opcode);
        break;
    case 0x33:
        bcdtomemFX33(opcode);
        break;
    case 0x55:
        regtomemFX55(opcode);
        break;
    case 0x65:
        memtoregFX65(opcode);
        break;
    default:
        printf("Unkown F group opcode %04x\n", opcode);
    }
}
void Chip8::gettimerFX07(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    V[x] = delay;
}
void Chip8::settimerFX15(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    delay = V[x];
}
void Chip8::setsoundFX18(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    sound = V[x];
}
void Chip8::addIFX1E(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    I += V[x];
}
void Chip8::waitinputFX0A(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    PC -= 2;

    for (int i = 0; i < 16; i++)
    {
        if (keys[i])
        {
            PC += 2;
            V[x] = i;
            break;
        }
    }
}
void Chip8::getFontCharFX29(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    I = 0x50 + V[x] * 5;
}
void Chip8::bcdtomemFX33(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t val = V[x];

    memory[I] = val / 100;
    memory[I + 1] = (val % 100) / 10;
    memory[I + 2] = val % 10;
}
void Chip8::regtomemFX55(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++)
    {
        memory[I + i] = V[i];
    }
}
void Chip8::memtoregFX65(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++)
    {
        V[i] = memory[I + i];
    }
}