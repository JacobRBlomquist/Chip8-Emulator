#ifndef CHIP_8_TEST_HPP
#define CHIP_8_TEST_HPP

//forward declare
class Chip8;

class Chip8Test
{
public:
    Chip8Test();
    ~Chip8Test();
public:
    // will return 0 if all tests pass.
    // Prints test results to stdout
    char * RunTests();

private:
   char * LoadFont();
   char * LoadROM();
   char * Fetch();

private:
    Chip8 *gChip8;
};

#endif //CHIP_8_TEST_HPP