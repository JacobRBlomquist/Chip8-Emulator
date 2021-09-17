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

void Chip8Test::RunTests()
{
    Run(&Chip8Test::Test1, "Test 1", "bad test");
}

void Chip8Test::Run(bool (Chip8Test::*pFunc)(), const char *pTestName, const char *pFailureMessage)
{
    bool res = std::invoke(pFunc, this);
    if (res)
    {
        printf("TEST '%s': PASS\n", pTestName);
    }
    else
    {
        printf("TEST '%s': FAIL\n\t-> %s\n", pTestName, pFailureMessage);
    }
}

bool Chip8Test::Test1()
{
    return gChip8->PC == 512;
}

int main(int argc, char **argv)
{
    Chip8 c8;

    Chip8Test test(&c8);
    test.RunTests();

    return 0;
}
