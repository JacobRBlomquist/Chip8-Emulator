/**
 * Copyright 2021 - Jacob R. Blomquist
 * Original Author: Jacob R. Blomquist <BlomDev at gmail dot com>
 * License: MIT
 */


#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <stdint.h>

class Chip8
{
public:
    Chip8();
    virtual ~Chip8();

    // Decode and execute a single CPU cycle
    void Cycle();
    // Load a ROM file - Returns 1 if an error occurred and 0 otherwise
    bool LoadRom(uint8_t *pRomData, uint32_t pRomSize);
    // Set keyboard state (keycode is [0,15], state is 0 or 1)
    void SetKeyState(uint8_t pKeyCode, uint8_t pState);
    // get screen buffer (64x32 bytes, 1 = on, 0 = off)
    const uint8_t *GetScreen();

private:
    // Reset processor registers, memory, etc
    void ResetState();
    // Fetch next instruction from Program Counter location
    uint16_t Fetch();
    // Decode an opcode and execute it
    void DecodeAndExecute(uint16_t pOpcode);

private:
    // 16 element call stack
    uint16_t stack[16];
    // Stack pointer
    uint16_t sp;
    // 4K memory
    uint8_t memory[4096];
    // 16 registers
    uint8_t V[16];
    // program counter
    uint16_t PC;
    // index register (for memory locations)
    uint16_t I;
    // delay timer. Counts down at 60hz until zero
    uint8_t delay;
    // keymap
    uint8_t keys[16];
    // sound timer, counts down at 60hz until zero. plays tone when non-zero
    uint8_t sound;
    // timer used to decrement sound and delay timers
    uint64_t lastTime;
    // constant for seconds per sound/delay tick
    const double secondsPer60Hz = 1.0 / 60.0;
    // used to accumulate time for sound/delay timers
    double unprocessedTime = 0;

    //display buffer
    uint8_t display[64 * 32];

    // 16 character font
    uint8_t font[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };


    //function pointer table
    void (Chip8::*FunctionTable[16])(uint16_t) = {
        &Chip8::zeroGroup,
        &Chip8::jmp1NNN,
        &Chip8::call2NNN,
        &Chip8::skip3XNN,
        &Chip8::skip4XNN,
        &Chip8::skip5XY0,
        &Chip8::set6XNN,
        &Chip8::add7XNN,
        &Chip8::eightGroup,
        &Chip8::skip9XY0,
        &Chip8::setIANNN,
        &Chip8::jmpoffBNNN,
        &Chip8::randCXNN,
        &Chip8::drawDXYN,
        &Chip8::egroup,
        &Chip8::fgroup};

    // opcodes that start at 0
    void zeroGroup(uint16_t opcode);
    // clear screen
    void cls00E0(uint16_t opcode);
    // return from function call
    void ret00EE(uint16_t opcode);

    // jump to address NNN
    void jmp1NNN(uint16_t opcode);

    // call function at NNN
    void call2NNN(uint16_t opcode);

    
    void skip3XNN(uint16_t opcode);
    void skip4XNN(uint16_t opcode);
    void skip5XY0(uint16_t opcode);
    void set6XNN(uint16_t opcode);
    void add7XNN(uint16_t opcode);

    void eightGroup(uint16_t opcode);
    void setxy8XY0(uint16_t opcode);
    void or8XY1(uint16_t opcode);
    void and8XY2(uint16_t opcode);
    void xor8XY3(uint16_t opcode);
    void add8XY4(uint16_t opcode);
    void sub8XY5(uint16_t opcode);
    void sub28XY7(uint16_t opcode);
    void shr8XY6(uint16_t opcode);
    void shl8XYE(uint16_t opcode);

    void skip9XY0(uint16_t opcode);

    void setIANNN(uint16_t opcode);
    void jmpoffBNNN(uint16_t opcode);
    void randCXNN(uint16_t opcode);
    void drawDXYN(uint16_t opcode);

    void egroup(uint16_t opcode);
    void skipkeyEX9E(uint16_t opcode);
    void skipnkeyEXA1(uint16_t opcode);

    void fgroup(uint16_t opcode);
    void gettimerFX07(uint16_t opcode);
    void settimerFX15(uint16_t opcode);
    void setsoundFX18(uint16_t opcode);
    void addIFX1E(uint16_t opcode);
    void waitinputFX0A(uint16_t opcode);
    void getFontCharFX29(uint16_t opcode);
    void bcdtomemFX33(uint16_t opcode);
    void regtomemFX55(uint16_t opcode);
    void memtoregFX65(uint16_t opcode);
};

#endif // CHIP8_HPP