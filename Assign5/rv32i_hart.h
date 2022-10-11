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
#include <string>
#include "memory.h"
#include "rv32i_decode.h"
#include "registerfile.h"

class rv32i_hart : public rv32i_decode
{
public:
    rv32i_hart(memory &m) : mem(m) { }
    void set_show_instructions(bool b) { show_instructions = b; }
    void set_show_registers(bool b) { show_registers = b; }
    bool is_halted() const { return halt; }
    const std::string &get_halt_reason() const { return halt_reason; }
    uint64_t get_insn_counter() const { return insn_counter; }
    void set_mhartid(int i) { mhartid = i; }

    void tick(const std::string &hdr="")
    void dump(const std::string &hdr="") const;
    void reset();

private:
    static contexpr int instruction_width           = 35;
    void exec(uint32_t insn, std::ostream*);
    void exec_illegal_insn(uint32_t insn, std::ostream*);
    //void exec_xxx(uint32_t insn, std::ostream*)

    bool halt = { false };
    bool show_instructions = { false };
    bool show_registers = { false };
    std::string halt_reason = { "none" };

    uint64_t insn_counter = { 0 };
    uint32_t pc = { 0 };
    uint32_t mhartid = { 0 };

protected:
    registerfile regs;
    memory &mem;
};

#endif