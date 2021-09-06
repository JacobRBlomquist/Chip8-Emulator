#ifndef CHIP8_HPP
#define CHIP8_HPP
#include <stdint.h>

class Chip8
{
public:
    Chip8();
    virtual ~Chip8();

    void Cycle();
    bool LoadRom(uint8_t *romData, uint32_t romSize);
    void SetKeyState(uint8_t keyCode, uint8_t state);
    const uint8_t *GetScreen();

private:
    void ResetState();
    uint16_t Fetch();
    void DecodeAndExecute(uint16_t opcode);

private:
    uint16_t stack[16];
    uint16_t sp;
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t PC;
    uint16_t I;
    uint8_t delay;
    uint8_t keys[16];
    uint8_t sound;
    uint64_t lastTime;
    double secondsPer60Hz = 1.0 / 60.0;
    double unprocessedTime = 0;


    uint8_t display[64 * 32];

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

    void zeroGroup(uint16_t opcode);
    void cls00E0(uint16_t opcode);
    void ret00EE(uint16_t opcode);

    void jmp1NNN(uint16_t opcode);
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