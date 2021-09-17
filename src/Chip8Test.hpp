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
    bool RunTests();

private:
    bool Test1();

private:
    Chip8 *gChip8;
};