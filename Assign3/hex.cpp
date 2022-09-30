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
#include <string>
#include <iomanip>
#include "hex.h"

/**
 * @brief Print 8 bit in hex.
 * 
 * Creates a stringstream object, formats, then extracts and returns formatted string.
 * 
 * @param i Unsigned 8bit integer to reformat to hexidecimal.
 * @return string representing integer in hexidecimal form.
 */
std::string hex::to_hex8(uint8_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
    return os.str();
}

/**
 * @brief Print 16 bit in hex.
 * 
 * Creates a stringstream object, formats, then extracts and returns formatted string.
 * 
 * @param i Unsigned 16bit integer to reformat to hexidecimal.
 * @return string representing integer in hexidecimal form.
 */
std::string hex::to_hex16(uint16_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(4) << i;
    return os.str();  
}

/**
 * @brief Print 32 bit in hex.
 * 
 * Creates a stringstream object, formats, then extracts and returns formatted string.
 * 
 * @param i Unsigned 16bit integer to reformat to hexidecimal.
 * @return string representing integer in hexidecimal form. 
 */
std::string hex::to_hex32(uint32_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(8) << i;
    return os.str();
}

/**
 * @brief Print 32 bit in hex with "0x" prefix.
 * 
 * Concatonates "0x" with the result of to_hex32().
 * 
 * @param i Unsigned 32bit integer to reformat to hexidecimal.
 * @return string representing integer in hexidecimal form with "0x" prefix.
 */
std::string hex::to_hex0x32(uint32_t i)
{
    return std::string("0x")+to_hex32(i);
}