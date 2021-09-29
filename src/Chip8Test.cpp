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
    mu_run_test(SetKeyState);
    mu_run_test(JMP);
    mu_run_test(Call);

    return 0;
}

char *Chip8Test::LoadFont()
{
    gChip8->ResetState();

    uint8_t ROM[] = {0, 2};

    gChip8->LoadRom(ROM, (uint32_t)sizeof(ROM));

    //first and last byte
    mu_assert("LoadFont - First byte of font is not 0xF0", gChip8->memory[0x50] == 0xF0);
    mu_assert("LoadFont - Last byte of font is not 0x80", gChip8->memory[0x9F] == 0x80);

    return 0;
}

char *Chip8Test::LoadROM()
{

    gChip8->ResetState();
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
    gChip8->ResetState();
    uint8_t ROM[]{0xff, 0xaa, 0xdf, 0xfa, 0xac};

    gChip8->LoadRom(ROM, sizeof(ROM));

    uint16_t opcode = gChip8->Fetch();
    mu_assert("Fetch - Wrong Opcode", opcode == 0xffaa);

    opcode = gChip8->Fetch();
    mu_assert("Fetch - Wrong Opcode", opcode == 0xdffa);

    return 0;
}

char *Chip8Test::SetKeyState()
{
    gChip8->ResetState();
    gChip8->SetKeyState(3, 1);

    mu_assert("Could not set keystate 3 to 1", gChip8->keys[3] == 1);

    gChip8->SetKeyState(5, 1);

    mu_assert("Could not set keystate 5 to 1", gChip8->keys[5] == 1);
    gChip8->SetKeyState(6, 1);
    mu_assert("Could not set keystate 6 to 1", gChip8->keys[6] == 1);
    gChip8->SetKeyState(6, 0);
    mu_assert("Could not set keystate 6 to 0", gChip8->keys[6] == 0);

    return 0;
}

char *Chip8Test::JMP()
{

    gChip8->ResetState();

    gChip8->jmp1NNN(0x1234);
    mu_assert("JMP incorrect address", gChip8->PC == 0x234);

    return 0;
}

char *Chip8Test::Call()
{
    gChip8->ResetState();

    gChip8->call2NNN(0x2123);
    mu_assert("Stack pointer is not 1", gChip8->sp == 1);
    mu_assert("Wrong Stack Value", gChip8->stack[0] == 0x200);
    mu_assert("PC is wrong", gChip8->PC == 0x123);

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
