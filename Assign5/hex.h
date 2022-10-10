#ifndef H_HEX
#define H_HEX

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

#include <string>

/**
 * @brief Hex Format Class
 * 
 * This class provides functions to facilitate siimpler formatting and printing of integers in hexidecimal forms.
 * 
 * @note to_hex16 was created as a bonus function past specification to use in cleaner program logic.
 */
class hex
{
public:
    static std::string to_hex8(uint8_t i);      //Print 8 bit in hex.
    static std::string to_hex16(uint16_t i);    //Print 16 bit in hex.
    static std::string to_hex32(uint32_t i);    //Print 32 bit in hex.
    static std::string to_hex0x32(uint32_t i);  //Print 32 bit in hex with an "0x" prefix.

    static std::string to_hex0x20(uint32_t i);  //Print 20 least significant bits  with an "0x" prefix.
    static std::string to_hex0x12(uint32_t i);  //Print 20 least significant bits  with an "0x" prefix.
};

#endif