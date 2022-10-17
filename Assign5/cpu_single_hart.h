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

/**
 * @brief Simulated Hardware Thread Class
 * 
 * Facilitates the creation and operation of a simulated CPU with a single hardware threads, allowing a decode/fetch/execute cycle.
 *
 */
class cpu_single_hart : public rv32i_hart
{
public:
    /**
     * @brief Construct a new cpu with a single hardware thread.
     * 
     * @param mem Size for the hardware thread object to use in initialization.
     */
    cpu_single_hart(memory &mem) : rv32i_hart(mem) {} //Constructor
    void run(uint64_t exec_limit);                    //Run simulated CPU.
};

#endif