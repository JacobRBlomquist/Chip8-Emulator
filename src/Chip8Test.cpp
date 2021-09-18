#include <cstdio>
#include <functional>
#include "Chip8Test.hpp"
#include "Chip8.hpp"

Chip8Test::Chip8Test(Chip8 *pChip8)
{
    gChip8 = pChip8;
}
Chip8Test::~Chip8Test()
{
}

bool Chip8Test::Run(bool (Chip8Test::*pFunc)(), const char *pTestName, const char *pFailureMessage)
{
    bool res = std::invoke(pFunc, this);
    if (res)
    {
        printf("TEST '%s':\tPASS\n", pTestName);
        return true;
    }

    printf("TEST '%s':\tFAIL\n\t-> %s\n", pTestName, pFailureMessage);
    return false;
}

void Chip8Test::RunTests()
{
    int total = 3;
    int passed = 0;

    Run(&Chip8Test::TestLoadFont, "Load Font", "Font Loading Failed") && passed++;
    Run(&Chip8Test::TestLoadRom, "Load ROM", "ROM Loading Failed") && passed++;
    Run(&Chip8Test::TestLoadRom, "Fetch", "Failed to fetch next opcode") && passed++;

    printf("Passed %d of %d\n", passed, total);
}

// font is loaded when rom is loaded
bool Chip8Test::TestLoadFont()
{

    uint8_t ROM[] = {0, 2};

    gChip8->LoadRom(ROM, (uint32_t)sizeof(ROM));

    //first and last byte
    return (gChip8->memory[0x50] == 0xF0 && gChip8->memory[0x9f] == 0x80);
}

bool Chip8Test::TestLoadRom()
{
    uint8_t ROM[] = {0xff, 0xaa, 0xdf, 0xfa, 0xac};

    gChip8->LoadRom(ROM, (uint32_t)sizeof(ROM));
    for (int i = 0; i < sizeof(ROM); i++)
    {
        if (ROM[i] != gChip8->memory[0x200 + i])
            return false;
    }

    return true;
}

bool Chip8Test::TestFetch()
{
    uint8_t ROM[]  {0xff, 0xaa, 0xdf, 0xfa, 0xac};

    gChip8->LoadRom(ROM,sizeof(ROM));

    uint16_t opcode = gChip8->Fetch();
    if(opcode!=0xffaa)
        return false;

    opcode = gChip8->Fetch();
     if(opcode!=0xdffa)
        return false;

    return true;
}

int main(int argc, char **argv)
{
    Chip8 c8;

    Chip8Test test(&c8);
    test.RunTests();

    return 0;
}
