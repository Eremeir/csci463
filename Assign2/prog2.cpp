//***************************************************************************
//
//  Matt Borek
//  z1951125
//  CSCI463-1
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//***************************************************************************

#include <iostream>
#include <iomanip>
#include <cmath>

/**
 * @brief Print Binary Float
 * 
 * Use input to calculate exponent and significand values for use in printing and calculation, then output binary to cout based on appropriate case and formatting.
 * 
 * @param x Unsigned 32 bit integer to be parsed and printed in floating binary representation.
 * 
 * @note x and the calculated values "exponent" and "significand" are not modified or shifted after assignment, and can be reused.
 */
void printBinFloat(uint32_t x)
{
    uint32_t hexMask = 0x80000000; //Hex mask for use in isolating the first bit.
    int32_t exponent = (((x & 0x7F800000) >> 23) - 127);  //Exponent portion of the float representation.
    uint32_t significand = (x & 0x007FFFFF); //Significand portion of the float representation. 23 bits long.
    std::cout << std::setfill('0');
    std::cout << "0x" << std::hex << std::setw(8) << x << " =";

    for (int i = 0; i < 32; i++) //Print each of the 32 bits of the provided hex.
    {
        if(i % 4 == 0) //Each 4 bits, print a space.
        {
            std::cout << " ";
        }
        std::cout << (x & hexMask ? '1':'0'); //Compare number and hex masks, print 1 if true, 0 otherwise.
        hexMask >>= 1; //Shift the hexmask to continue comparrison.
    }
    std::cout << std::endl << "sign: " << (x & 0x80000000 ? '1':'0') << std::endl; //Compare first bit and hex mask to produce the sign bit.
    
    std::cout << " exp: 0x" << std::hex << std::setw(8) << exponent << std::dec << " (" << exponent << ")" << std::endl; //Print the exponent in hex and decimal.
    std::cout << " sig: 0x" << std::hex << std::setw(8) << significand << std::endl; //Print the significand in hex.

    std::cout << (x & 0x80000000 ? '-':'+'); //Apply a + or - depending on the signbit of the number.

    if(!(x & 0x7F800000) && (significand == 0)) //Test that the exponent and significand are all zeroes.
    {                    //!(significand | 0x00000000) Bitwise Alternate
        std::cout << "0" << std::endl; //This condition presents as the zero case.
    }

    else if(((x & 0x7F800000) == 0x7F800000) && !(significand | 0x00000000)) //Test that the exponent is all ones and the significand is all zeroes.
    {
        std::cout << "inf" << std::endl; //This condition presents as the infinity case.
    }
    else //If not a special case, print the number.
    {
        if(exponent == 0) //If the exponent is 0.
        {
            std::cout << "1.";
            hexMask = 0x00400000; //Hexmask to check starting at the first significand bit.
            for (int j = 0; j < 23; j++)
            {
                std::cout << (significand & hexMask ? '1':'0'); //Compare number and hex masks, print 1 if true, 0 otherwise.
                hexMask >>= 1; //Shift the hexmask to continue comparrison.
            }
            std::cout << std::endl;
        }

        else if(exponent < 0) //If the exponent is negative.
        {
            std::cout << "0." ;
            for (int j = abs(exponent) - 1; j > 0; j--) //Print a number of zeroes equal to the absolute value of the exponent
            {                                           //minus the one already printed with the binary point.
                std::cout << "0";
            }

            std::cout << "1";
            hexMask = 0x00400000; //Hexmask to check starting at the first significand bit.
            for (int k = 0; k < 23; k++)
            {
                std::cout << (significand & hexMask ? '1':'0'); //Compare number and hex masks, print 1 if true, 0 otherwise.
                hexMask >>= 1; //Shift the hexmask to continue comparrison.
            }
            std::cout << std::endl;
        }

        else if(exponent > 0) //If the exponent is positive.
        {
            std::cout << "1";
            hexMask = 0x00400000; //Hexmask to check starting at the first significand bit.
            for (int j = 0; j < 23; j++)
            {
                std::cout << (significand & hexMask ? '1':'0'); //Compare number and hex masks, print 1 if true, 0 otherwise.
                hexMask >>= 1; //Shift the hexmask to continue comparrison.
            }

            for (int k = exponent - 23; k > 0; k--) //Print a number of zeroes equal to the exponent minus the length of the significand.
            {
                std::cout << "0";
            }

            std::cout << ".0" << std::endl;
        }
    } 
}

/**
 * @brief Main subroutine.
 * 
 * Accept valid 32 bit integer input from cin and call the appropriate float print function.
 * 
 * @return int 0 to signal end of program.
 */
int main()
{
    uint32_t x;
    while(std::cin >> std::hex >> x)
    {
        printBinFloat(x);
    }

    return 0;
}