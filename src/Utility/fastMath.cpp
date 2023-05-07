
#include "../Utility/fastMath.h"

uint16_t fastSquareRoot32BitInput(int input)
{
    return squareRootMapFor14MsbOf32BitInputs[input >> 18];
}