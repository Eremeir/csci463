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
#include "cpu_single_hart.h"

/**
 * @brief Run simulated CPU.
 *
 * Simulate a CPU and run executable program.
 * 
 * @param exec_limit Limit of instructions to execute.
 */
void cpu_single_hart::run(uint64_t exec_limit)
{
    regs.set(2, mem.get_size()); //Set register x2 to the maximum memory size.
    if(exec_limit == 0)
    {
        while(!is_halted()) //While the hardware thread isn't halted.
        {
            tick();
        }
    }
    else
    {
        while(!is_halted() && (get_insn_counter() < exec_limit)) //While the hardware thread isn't halted and thread isn't as execution limit.
        {
            tick();
        }
    }

    if(is_halted())
    {
        std::cout << get_halt_reason() << std::endl;
    }

    std::cout << get_insn_counter() << " instructions executed" << std::endl;
}