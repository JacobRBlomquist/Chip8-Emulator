#ifndef CHIP_8_TEST_HPP
#define CHIP_8_TEST_HPP

//forward declare
class Chip8;

class Chip8Test
{
public:
    Chip8Test(Chip8 *pChip8);
    virtual ~Chip8Test();

public:
    // will return 0 if all tests pass.
    // Prints test results to stdout
    void RunTests();

private:
    //Run a specific test based on function pointer.
    // If test passes print pass, otherwise print fail.
    void Run(bool (Chip8Test::*pFunc)(), const char *pTestName, const char *pFailureMessage);

    bool Test1();

private:
    Chip8 *gChip8;
};

#endif //CHIP_8_TEST_HPP