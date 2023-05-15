
#include "../Utility/fastMath.h"

uint8_t fastSquareRoot16BitInput(uint16_t input)
{
    return squareRootMapFor14MsbOf16BitInput[input >> 2];
}

uint8_t fastSquareRootOnlyProvide14MsbOf16BitInput(uint16_t input)
{
    return squareRootMapFor14MsbOf16BitInput[input];
}