#ifndef H_HART
#define H_HART

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
//#include <string>
#include "memory.h"
#include "rv32i_decode.h"
#include "registerfile.h"

/**
 * @brief Simulated Hardware Thread Class
 * 
 * Facilitates the creation and operation of simulated hardware threads, allowing execution of binary code.
 *
 */
class rv32i_hart : public rv32i_decode
{
public:
    /**
     * @brief Construct a new rv32i hardware thread.
     * 
     * @param m Size for the memory object to use in initializing the hardware thread.
     */
    rv32i_hart(memory &m) : mem(m) { }           //Constructor
    void set_show_instructions(bool b);          //Set the show instructions flag.
    void set_show_registers(bool b);             //Set the show registers flag.
    bool is_halted() const;                      //Return halt status.
    const std::string& get_halt_reason() const;  //Return halt reason.
    uint64_t get_insn_counter() const;           //Get instruction counter.
    void set_mhartid(int ID);                    //Set mhart ID.

    void tick(const std::string &hdr="");        //Tick instruction execution.
    void dump(const std::string &hdr="") const;  //Dump hardware thread.
    void reset();                                //Reset hardware thread.

private:
    static constexpr int instruction_width           = 35;
    void exec(uint32_t insn, std::ostream* pos);              //Dump hardware thread.
    void exec_illegal_insn(uint32_t insn, std::ostream* pos); //Reset hardware thread.
    //void exec_xxx(uint32_t insn, std::ostream*)

    bool halt = { false };
    bool show_instructions = { false };
    bool show_registers = { false };
    std::string halt_reason = { "none" };

    uint64_t insn_counter = { 0 };
    uint32_t pc = { 0 };
    uint32_t mhartid = { 0 };

protected:
    registerfile regs; //Vector to simulate registers.
    memory &mem;       //Vector to simulate memory.
};

#endif