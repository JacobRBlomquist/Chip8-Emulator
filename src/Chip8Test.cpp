#include "Chip8.hpp"
#include <cstdio>

Chip8Test::Chip8Test(Chip8 *pChip8)
{
    gChip8 = pChip8;
}
Chip8Test::~Chip8Test()
{
}

bool Chip8Test::RunTests()
{
    return Test1();
}

bool Chip8Test::Test1()
{
    printf("PC: %d",gChip8->PC);

    return true;
}


int main(int argc, char** argv){
    Chip8 c8;

    Chip8Test test(&c8);
    test.RunTests();

    return 0;
}
