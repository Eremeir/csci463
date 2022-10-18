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
#include <cassert>
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
        std::cout << hdr << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
        exec(insn, &std::cout);
    }
    else
    {
        exec(insn, nullptr);
    }
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
 * @brief Execute instruction.
 *
 * Decode and execute instruction, optionally render them.
 * 
 * @param insn Instruction to execute.
 * @param pos Output to send anything to print.
 */
void rv32i_hart::exec(uint32_t insn, std::ostream* pos)
{
    switch(get_opcode(insn)) //Render based on determined opcode.
    {
        default:                exec_illegal_insn(insn,pos); return;
        case opcode_lui:        exec_lui(insn, pos); return;
        case opcode_auipc:      exec_auipc(insn, pos); return;
        case opcode_jal:        exec_jal(insn, pos); return;
        case opcode_jalr:       exec_jalr(insn, pos); return;
        /*
        case opcode_btype:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:            return render_illegal_insn(insn);
            case funct3_beq:    return render_btype(addr, insn, "beq");
            case funct3_bne:    return render_btype(addr, insn, "bne");
            case funct3_blt:    return render_btype(addr, insn, "blt");
            case funct3_bge:    return render_btype(addr, insn, "bge");
            case funct3_bltu:   return render_btype(addr, insn, "bltu");
            case funct3_bgeu:   return render_btype(addr, insn, "bgeu");
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_load_imm:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:                return render_illegal_insn(insn);
            case funct3_lb:         return render_itype_load(insn, "lb");
            case funct3_lh:         return render_itype_load(insn, "lh");
            case funct3_lw:         return render_itype_load(insn, "lw");
            case funct3_lbu:        return render_itype_load(insn, "lbu");
            case funct3_lhu:        return render_itype_load(insn, "lhu");
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_stype:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:                return render_illegal_insn(insn);
            case funct3_sb:         return render_stype(insn, "sb");
            case funct3_sh:         return render_stype(insn, "sh");
            case funct3_sw:         return render_stype(insn, "sw");
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_alu_imm:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:                return render_illegal_insn(insn);
            case funct3_add:        return render_itype_alu(insn, "addi", get_imm_i(insn));
            case funct3_slt:        return render_itype_alu(insn, "slti", get_imm_i(insn));
            case funct3_sltu:       return render_itype_alu(insn, "sltiu", get_imm_i(insn));
            case funct3_xor:        return render_itype_alu(insn, "xori", get_imm_i(insn));
            case funct3_or:         return render_itype_alu(insn, "ori", get_imm_i(insn));
            case funct3_and:        return render_itype_alu(insn, "andi", get_imm_i(insn));

            case funct3_sll:        return render_itype_alu(insn, "slli", get_imm_i(insn)%XLEN);
            case funct3_srx:
            switch(get_funct7(insn)) //Discriminate further by funct7.
            {
                default:            return render_illegal_insn(insn);
                case funct7_sra:    return render_itype_alu(insn, "srai", get_imm_i(insn)%XLEN);
                case funct7_srl:    return render_itype_alu(insn, "srli", get_imm_i(insn)%XLEN);
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_rtype:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:                return render_illegal_insn(insn);
            case funct3_sll:        return render_rtype(insn, "sll");
            case funct3_slt:        return render_rtype(insn, "slt");
            case funct3_sltu:       return render_rtype(insn, "sltu");
            case funct3_xor:        return render_rtype(insn, "xor");
            case funct3_or:         return render_rtype(insn, "or");
            case funct3_and:        return render_rtype(insn, "and");

            case funct3_add:
            switch(get_funct7(insn))
            {
                default:            return render_illegal_insn(insn);
                case funct7_add:    return render_rtype(insn, "add");
                case funct7_sub:    return render_rtype(insn, "sub");
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
            
            case funct3_srx:
            switch(get_funct7(insn)) //Discriminate further by funct7.
            {
                default:            return render_illegal_insn(insn);
                case funct7_sra:    return render_rtype(insn, "sra");
                case funct7_srl:    return render_rtype(insn, "srl");
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_system:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:                return render_illegal_insn(insn);
            
            case eCode:
            switch(insn) //Check if instruction matches system ecodes.            
            {
                default:            return render_illegal_insn(insn);
                case insn_ecall:    return render_ecall(insn);
                case insn_ebreak:   return render_ebreak(insn);
            }
            assert(0 && "unrecognized code"); //It should be impossible to ever get here!

            case funct3_csrrw:      return render_csrrx(insn, "csrrw");
            case funct3_csrrs:      return render_csrrx(insn, "csrrs");
            case funct3_csrrc:      return render_csrrx(insn, "csrrc");

            case funct3_csrrwi:     return render_csrrxi(insn, "csrrwi");
            case funct3_csrrsi:     return render_csrrxi(insn, "csrrsi");
            case funct3_csrrci:     return render_csrrxi(insn, "csrrci");
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!
    }
    assert(0 && "unrecognized opcode"); //It should be impossible to ever get here! */
    }
}

/**
 * @brief Illegal Instruction Subroutine.
 * 
 * Output a message indicating the instruction could not execute and halt the hardware thread.
 * 
 * @param insn Instruction that was unable to execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
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


/**
 * @brief Execute lui.
 *
 * Execute the lui instruction, setting the register to the imm_u.
 * 
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 */
void rv32i_hart::exec_lui(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t val = (get_imm_u(insn));

    if(pos)
    {
        std::string s = render_lui(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex :: to_hex0x32(val) << std::endl;
    }

    regs.set(rd , val);
    pc += 4;
}

/**
 * @brief Execute auipc.
 *
 * Execute the auipc instruction, adding the instruction address to the imm_u.
 * 
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 */
void rv32i_hart::exec_auipc(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);
    int32_t val = (imm_u + pc);

    if(pos)
    {
        std::string s = render_auipc(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex :: to_hex0x32(pc) << " + " << hex::to_hex0x32(imm_u) << " = " << hex::to_hex0x32(val) << std::endl;
    }

    regs.set(rd , val);
    pc += 4;
}

/**
 * @brief Execute jal.
 *
 * Execute the jal instruction, setting register to the address of the next instruction that would otherwise be executed 
 * and jumping to the address given by the sum of the pc register and the imm_j.
 *
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 */
void rv32i_hart::exec_jal(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_j = get_imm_j(insn);
    int32_t val = (imm_j + pc);

    if(pos)
    {
        std::string s = render_jal(pc,insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex :: to_hex0x32(pc + 4) << ",  pc = " << hex::to_hex0x32(pc) << " + " << hex::to_hex0x32(imm_j);
        *pos << " = " <<  hex::to_hex0x32(val) << std::endl;
    }

    regs.set(rd , pc + 4);
    pc = val;
}

/**
 * @brief Execute jalr.
 *
 * Execute the jalr instruction, setting register to the address of the next instruction that would otherwise be executed 
 * and jumping to the address given by the sum of the rs1 register and the imm_i.
 *
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 */
void rv32i_hart::exec_jalr(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);
    int32_t val = ((imm_i + regs.get(rs1)) & 0xfffffffe);

    if(pos)
    {
        std::string s = render_jalr(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex :: to_hex0x32(pc + 4) << ",  pc = (" << hex::to_hex0x32(imm_i) << " + " << hex::to_hex0x32(regs.get(rs1));
        *pos << ") & 0xfffffffe = " <<  hex::to_hex0x32(val) << std::endl;
    }

    regs.set(rd , pc + 4);
    pc = val;
}