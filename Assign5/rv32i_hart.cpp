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
#include "rv32i_hart.h"

/**
 * @brief Set the show instructions flag.
 *
 * When flag is true, show each instruction that is executed with a display of register values used.
 * 
 * @param bool indicating whether the flag should be on or off.
 */
void rv32i_hart::set_show_instructions(bool b)
{ 
    show_instructions = b;
}

/**
 * @brief Set the show registers flag.
 * 
 * When flag is true, dump the registers before each instruction is executed.
 *
 * @param bool indicating whether the flag should be on or off.
 */
void rv32i_hart::set_show_registers(bool b)
{
    show_registers = b;
}

/**
 * @brief Return halt status.
 * 
 * @return true if the hardware thread is halted for any reason.
 * @return false if the hardware thread is not halted.
 */
bool rv32i_hart::is_halted() const
{
    return halt;
}

/**
 * @brief Get halt reason.
 * 
 * Possible halt reasons include:
 * "none"
 * "EBREAK instruction"
 * "ECALL instruction"
 * "Illegal CSR in CRRSS instruction"
 * "Illegal instruction"
 * "PC alignment error"
 *
 * @return String of current reason for halt.
 */
const std::string& rv32i_hart::get_halt_reason() const
{
    return halt_reason;
}

/**
 * @brief Get instruction counter.
 * 
 * @return Number of instructions executed since last reset.
 */
uint64_t rv32i_hart::get_insn_counter() const
{ 
    return insn_counter; 
}

/**
 * @brief Set mhart ID.
 * 
 * @param ID number to set.
 */
void rv32i_hart::set_mhartid(int ID)
{
    mhartid = ID;
}

/**
 * @brief Tick instruction execution.
 *
 * Tell the simulator to start execution of an instruction.
 * 
 * @param hdr String to be printed to the left of any output.
 */
void rv32i_hart::tick(const std::string &hdr)
{
    if(is_halted())
    {   
        return;
    }
    
    if(show_registers)
    {
        dump(hdr);
    }

    if(!(pc % 4 == 0))
    {
        halt = true;
        halt_reason = "PC alignment error";
        return;
    }

    insn_counter++;

    uint32_t insn = mem.get32(pc);

    if(show_instructions)
    {
        std::cout << hdr << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << std::endl;
        exec(insn, &std::cout);
    }
    else
    {
        exec(insn, nullptr);
    }
    pc += 4;
}

/**
 * @brief Dump hardware thread.
 *
 * Dump the state of the hardware thread.
 * 
 * @param hdr String to be printed to the left of any output.
 */
void rv32i_hart::dump(const std::string &hdr) const
{

}

/**
 * @brief Reset hardware thread.
 *
 * Reset the state of the hardware thread by setting the pc, registers, and hart flags to their initial state.
 * 
 */
void rv32i_hart::reset()
{
    pc = 0;
    regs.reset();
    insn_counter = 0;
    halt = false;
    halt_reason = "none";
}

/**
 * @brief Execute Instruction.
 *
 * Decode and execute instruction, optionally render them.
 * 
 * @param insn Instruction to execute.
 * @param pos Output to send anything to print.
 */
void rv32i_hart::exec(uint32_t insn, std::ostream* pos)
{

}

/**
 * @brief Illegal Instruction Subroutine.
 * 
 * Output a message indicating the instruction could not execute and halt the hardware thread.
 * 
 * @param insn Instruction that was unable to execute.
 * @param pos Output to send instruction information to print.
 */
void rv32i_hart::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
    if(pos)
    {
        *pos << render_illegal_insn(insn);
    }
    halt = true;
    halt_reason = "Illegal instruction";
}
//void exec_xxx(uint32_t insn, std::ostream*)