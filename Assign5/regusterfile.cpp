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
#include "registerfile.h"

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
registerfile~registerfile()
{

} 

void registerfile::reset()
{
    regs.assign(32, 0xf0f0f0f0);
    regs.at(0) = 0; //Initialize register x0 to zero,
}

void registerfile::set(uint32_t reg, int32_t val)
{
    if(reg != 0) //Discard values sent to register 0.
    {
        regs.at(reg) = val; //Assign register the given val.
    }
}
int32_t registerfile::get(uint32_t reg)
{
    return regs.at(reg);
} 
void registerfile::dump(const std::string &hdr)
{
    
}    //Dump register contents.
    