#include <cstdio>
#include <functional>
#include "minUnit.hpp"
#include "Chip8Test.hpp"
#include "Chip8.hpp"

int tests_run = 0;

Chip8Test::Chip8Test()
{
    gChip8 = new Chip8();
}
Chip8Test::~Chip8Test()
{
    delete gChip8;
}

char *Chip8Test::RunTests()
{

    mu_run_test(LoadROM);
    mu_run_test(LoadFont);
    mu_run_test(Fetch);

    return 0;
}

char *Chip8Test::LoadFont()
{

    uint8_t ROM[] = {0, 2};

    gChip8->LoadRom(ROM, (uint32_t)sizeof(ROM));

    //first and last byte
    mu_assert("LoadFont - First byte of font is not 0xF0", gChip8->memory[0x50] == 0xF0);
    mu_assert("LoadFont - Last byte of font is not 0x80", gChip8->memory[0x9F] == 0x80);

    return 0;
}

char *Chip8Test::LoadROM()
{

    uint8_t ROM[] = {0xff, 0xaa, 0xdf, 0xfa, 0xac};

    gChip8->LoadRom(ROM, (uint32_t)sizeof(ROM));

    for (int i = 0; i < sizeof(ROM); i++)
    {
        mu_assert("LoadRom - Memory does not match ROM after loading.", ROM[i] == gChip8->memory[0x200 + i]);
    }

    return 0;
}

char *Chip8Test::Fetch()
{
    uint8_t ROM[]{0xff, 0xaa, 0xdf, 0xfa, 0xac};

    gChip8->LoadRom(ROM, sizeof(ROM));

    uint16_t opcode = gChip8->Fetch();
    mu_assert("Fetch - Wrong Opcode", opcode == 0xffaa);

    opcode = gChip8->Fetch();
    mu_assert("Fetch - Wrong Opcode", opcode == 0xdffa);

    return 0;
}


int main(int argc, char **argv)
{

    Chip8Test test;
    char *result = test.RunTests();

    if (result != 0)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }

    printf("Tests Run: %d\n", tests_run);

    return 0;
}
