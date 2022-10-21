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
#include "registerfile.h"

/**
 * @brief Construct a new registerfile.
 *
 * Resize the register and set the contents to initial values.
 * 
 */
registerfile::registerfile()
{
    regs.resize(32);
    reset();
}

/**
 * @brief Destroy the register vector.
 * 
 * @note Redundant, done automatically at end of scope.
 * 
 */
registerfile::~registerfile()
{

} 

/**
 * @brief Reset and initialize registers.
 *
 * Set every register to 0xf0f0f0f0 except for x0, which is permanently zero.
 * 
 */
void registerfile::reset()
{
    regs.assign(32, 0xf0f0f0f0);
    regs.at(0) = 0; //Initialize register x0 to zero,
}

/**
 * @brief Set register value.
 *
 * Set the contents of a register, except for x0.
 * 
 * @param reg register number to access.
 * @param val value to write into the register.
 */
void registerfile::set(uint32_t reg, int32_t val)
{
    if(reg != 0) //Discard values sent to register 0.
    {
        regs.at(reg) = val; //Assign register the given val.
    }
}

/**
 * @brief Return register value.
 * 
 * @param reg Regoster number to fetch from.
 * @return Value of register contents.
 */
int32_t registerfile::get(uint32_t reg) const
{
    return regs.at(reg);
}

/**
 * @brief Dump register contents.
 * 
 * @param hdr String to be printed to the left of any output.
 */
void registerfile::dump(const std::string &hdr) const
{
    uint32_t regCount = 0; //Number of registers printed.
    for(uint32_t i = 0; i < regs.size(); i+=8) //Iterate once per line for a set of 8 registers.
    {
        if(hdr != "") //If prefix string isn't blank.
        {
            std::cout << hdr;
        }
        std::cout << std::right << std::setw(3) << "x" + std::to_string(regCount) << " ";
        for(uint32_t j = 0; j < 8; ++j) //Iterate each register.
        {
            std::cout << hex::to_hex32(get(regCount)); //Print the register value.
            if(j < 7)  
            {
                std::cout  << " ";
                if(j != 0 && (j + 1) % 4 == 0) //Add a space every 4 registers.
                {
                    std::cout  << " ";
                }
            }
            ++regCount; 
        }
        std::cout << std::endl;
    }
}
    