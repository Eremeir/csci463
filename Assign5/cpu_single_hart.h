#ifndef H_CPU
#define H_CPU

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
#include "rv32i_hart.h"

class cpu_single_hart : public rv32i_hart
{
public:
    cpu_single_hart(memory &mem) : rv32i_hart(mem) {}
    void run(uint64_t exec_limit);

};

#endif