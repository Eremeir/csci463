#ifndef H_REGISTER
#define H_REGISTER

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
#include <vector>
#include "hex.h"

class registerfile : public hex
{
public:
    registerfile(); //Initialize register x0 to zero, and all other registers to 0xf0f0f0f0.
    ~registerfile(); 

    void set(uint32_t reg, int32_t val); //Assign register r the given val. If r is zero then do nothing.
    int32_t get(uint32_t  reg); //Return the value of register r. If r is zero then return zero.
    void dump(const std::string &hdr); //Dump register contents.
    
private:
    std::vector<int32_t> regs; //Vector to simulate registers.
};

#endif