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
    uint32_t funct3 = get_funct3(insn);
    switch(get_opcode(insn)) //Render based on determined opcode.
    {
        default:                exec_illegal_insn(insn,pos); return;
        case opcode_lui:        exec_lui(insn, pos); return;
        case opcode_auipc:      exec_auipc(insn, pos); return;
        case opcode_jal:        exec_jal(insn, pos); return;
        case opcode_jalr:       exec_jalr(insn, pos); return;
        
        case opcode_btype:      
        switch(funct3) //Discriminate further by funct3.
        {
            default:            exec_illegal_insn(insn, pos); return;
            case funct3_beq:    exec_btype(insn, pos, funct3, "beq"); return;
            case funct3_bne:    exec_btype(insn, pos, funct3, "bne"); return;
            case funct3_blt:    exec_btype(insn, pos, funct3, "blt"); return;
            case funct3_bge:    exec_btype(insn, pos, funct3, "bge"); return;
            case funct3_bltu:   exec_btype(insn, pos, funct3, "bltu"); return;
            case funct3_bgeu:   exec_btype(insn, pos, funct3, "bgeu"); return;
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!
        
        case opcode_load_imm:   
        switch(funct3) //Discriminate further by funct3.
        {
            default:                exec_illegal_insn(insn, pos); return;
            case funct3_lb:         exec_itype_load(insn, pos, funct3, "lb"); return;
            case funct3_lh:         exec_itype_load(insn, pos, funct3, "lh"); return;
            case funct3_lw:         exec_itype_load(insn, pos, funct3, "lw"); return;
            case funct3_lbu:        exec_itype_load(insn, pos, funct3, "lbu"); return;
            case funct3_lhu:        exec_itype_load(insn, pos, funct3, "lhu"); return;
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!
        
        case opcode_stype:       
        switch(funct3) //Discriminate further by funct3.
        {
            default:                exec_illegal_insn(insn, pos); return;
            case funct3_sb:         exec_stype(insn, pos, funct3, "sb"); return;
            case funct3_sh:         exec_stype(insn, pos, funct3, "sh"); return;
            case funct3_sw:         exec_stype(insn, pos, funct3, "sw"); return;
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_alu_imm:       
        switch(funct3) //Discriminate further by funct3.
        {
            default:                exec_illegal_insn(insn, pos); return;
            case funct3_add:        exec_itype_alu(insn, pos, funct3, "addi"); return;
            case funct3_slt:        exec_itype_alu(insn, pos, funct3, "slti"); return;
            case funct3_sltu:       exec_itype_alu(insn, pos, funct3, "sltiu"); return;
            case funct3_xor:        exec_itype_alu(insn, pos, funct3, "xori"); return;
            case funct3_or:         exec_itype_alu(insn, pos, funct3, "ori"); return;
            case funct3_and:        exec_itype_alu(insn, pos, funct3, "andi"); return;

            case funct3_sll:        exec_itype_alu(insn, pos, funct3, "slli"); return;
            case funct3_srx:
            switch(get_funct7(insn)) //Discriminate further by funct7.
            {
                default:            exec_illegal_insn(insn, pos); return;
                case funct7_sra:    exec_itype_alu(insn, pos, funct3, "srai"); return;
                case funct7_srl:    exec_itype_alu(insn, pos, funct3, "srli"); return;
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!
        /*
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
    if(pos) //If output stream exists.
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
    int32_t val = (get_imm_u(insn)); //Set register rd to the imm_u value.

    if(pos) //If output stream exists.
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
    int32_t val = (imm_u + pc); //Add the address of the instruction to the imm_u value and store the result in register rd.

    if(pos) //If output stream exists.
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
    int32_t val = (imm_j + pc); //Set register rd to address of next instruction then jump to address given by sum of the pc register and imm_j.

    if(pos) //If output stream exists.
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
    uint32_t rs1Con = regs.get(get_rs1(insn)); //Contents of rs1.
    int32_t imm_i = get_imm_i(insn);
    int32_t val = ((imm_i + rs1Con) & 0xfffffffe); //Set register rd to address of next instruction, jump to address of rs1 register + imm_i value.

    if(pos) //If output stream exists.
    {
        std::string s = render_jalr(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex :: to_hex0x32(pc + 4) << ",  pc = (" << hex::to_hex0x32(imm_i) << " + " << hex::to_hex0x32(rs1Con);
        *pos << ") & 0xfffffffe = " <<  hex::to_hex0x32(val) << std::endl;
    }

    regs.set(rd , pc + 4);
    pc = val;
}

/**
 * @brief Execute B Type instruction.
 *
 * Execute between several B Type instructions based on funct3 code.
 * 
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 * @param funct3 Value to discriminate b-type instructions.
 * @param mnemonic String to pass to rendering function.
 */
void rv32i_hart::exec_btype(uint32_t insn, std::ostream* pos, uint32_t funct3, const char *mnemonic)
{
    uint32_t rs1Con = regs.get(get_rs1(insn)); //Contents of rs1.
    uint32_t rs2Con = regs.get(get_rs2(insn)); //Contents of rs2.
    int32_t imm_b = get_imm_b(insn);
    int32_t val; //Value to adjust pc register.

    if(pos) //If output stream exists.
    {
        std::string s = render_btype(pc, insn, mnemonic);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
    }
    
    switch(funct3)
    {
        default:            exec_illegal_insn(insn, pos); return;
        case funct3_beq:
        {
            val = ((rs1Con == rs2Con) ? imm_b : 4); //If rs1 is equal to rs2 then add imm_b to pc register, otherwise 4.
            if(pos) 
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " == " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val) << std::endl;
            }
        }
        break;

        case funct3_bne:
        {
            val = ((rs1Con != rs2Con) ? imm_b : 4); //If rs1 is not equal to rs2 then add imm_b to pc register, otherwise 4.
            if(pos) 
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " != " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val)<< std::endl;
            }
        }
        break;

        case funct3_blt:
        {

            val = ((static_cast<int32_t>(rs1Con) < static_cast<int32_t>(rs2Con)) ? imm_b : 4); //If signed val in rs1 is less than signed val in rs2 
            if(pos)                                                                            //then add imm_b to pc register, otherwise 4.
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " < " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val) << std::endl;
            }
        }
        break;

        case funct3_bge:
        {
            val = ((static_cast<int32_t>(rs1Con) >= static_cast<int32_t>(rs2Con)) ? imm_b : 4); //If signed val in rs1 is greater than or equal to 
            if(pos)                                                                             //signed val in rs2 then add imm_b to pc register, otherwise 4.
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " >= " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val) << std::endl;
            }
        }
        break;

        case funct3_bltu: 
        {
             val = ((rs1Con < rs2Con) ? imm_b : 4); //If unsigned val in rs1 is less than unsigned val in rs2 then add imm_b to pc register, otherwise 4.
            if(pos)
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " <U " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val) << std::endl;
            }
        }
        break;

        case funct3_bgeu:
        {
            val = ((rs1Con >= rs2Con) ? imm_b : 4); //If unsigned val in rs1 is greater than or equal to 
            if(pos)                                 //unsigned val in rs2 then add imm_b to pc register, otherwise 4.
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " >=U " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val) << std::endl;
            }
        }
        break;
    }

    pc += val;
}

/**
 * @brief Execute I Type-LOAD instruction.
 *
 * Execute between several I Type-LOAD instructions based on funct3 code.
 * 
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 * @param funct3 Value to discriminate I Type-LOAD instructions.
 * @param mnemonic String to pass to rendering function.
 */
void rv32i_hart::exec_itype_load(uint32_t insn, std::ostream* pos, uint32_t funct3, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1Con = regs.get(get_rs1(insn)); //Contents of rs1.
    int32_t imm_i = get_imm_i(insn);
    int32_t val; //Value to set register.

    if(pos) //If output stream exists.
    {
        std::string s = render_itype_load(insn, mnemonic);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
    }
    
    switch(funct3)
    {
        default:            exec_illegal_insn(insn, pos); return;
        case funct3_lb:
        {
            val = mem.get8_sx(rs1Con + imm_i); //Set register rd to value of sign-extended byte fetched from memory address given by sum of rs1 and imm_i.
            if(pos)
            {
                *pos << "// " << render_reg(rd) << " = sx(m8(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << ")) = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_lh:
        {
            val = mem.get16_sx(rs1Con + imm_i); //Set register rd to value of sign-extended 16-bit little-endian half-word value
            if(pos)                             //from memory address given by sum of rs1 and imm_i.
            {
                *pos << "// " << render_reg(rd) << " = sx(m16(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << ")) = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_lw:
        {
            val = mem.get32_sx(rs1Con + imm_i); //Set register rd to value of sign-extended 32-bit little-endian word value
            if(pos)                             //from memory address given by sum of rs1 and imm_i.
            {
                *pos << "// " << render_reg(rd) << " = sx(m32(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << ")) = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_lbu:
        {
            val = mem.get8(rs1Con + imm_i); //Set register rd to value of zero-extended byte from memory address given by sum of rs1 and imm_i.
            if(pos)
            {
                *pos << "// " << render_reg(rd) << " = zx(m8(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << ")) = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_lhu: 
        {
             val = mem.get16(rs1Con + imm_i); //Set register rd to value of zero-extended 16-bit little-endian half-word value
            if(pos)                           //from memory address given by sum of rs1 and imm_i.
            {
                *pos << "// " << render_reg(rd) << " = zx(m16(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << ")) = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;
    }

    regs.set(rd , val);
    pc += 4;
}

/**
 * @brief Execute S Type instruction.
 *
 * Execute between several S Type instructions based on funct3 code.
 * 
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 * @param funct3 Value to discriminate S Type instructions.
 * @param mnemonic String to pass to rendering function.
 */
void rv32i_hart::exec_stype(uint32_t insn, std::ostream* pos, uint32_t funct3, const char *mnemonic)
{
    uint32_t rs1Con = regs.get(get_rs1(insn)); //Contents of rs1.
    uint32_t rs2Con = regs.get(get_rs2(insn)); //Contents of rs2.
    int32_t imm_s = get_imm_s(insn);
    uint32_t addr = (rs1Con + imm_s); //Address to set memory.

    if(pos) //If output stream exists.
    {
        std::string s = render_stype(insn, mnemonic);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
    }

    switch(funct3)
    {
        default:            exec_illegal_insn(insn, pos); return;
        case funct3_sb:
        {
            mem.set8(addr, rs2Con & 0x000000ff); //Set byte of memory at address given by sum of rs1 and imm_s to 8 LSBs of rs2.
            if(pos)
            {
                *pos << "// m8(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_s) << ") = ";
                *pos << hex::to_hex0x32(rs2Con & 0x000000ff) << std::endl;
            }
        }
        break;

        case funct3_sh:
        {
            mem.set16(addr, rs2Con& 0x0000ffff); //Set 16-bit half-word of memory at address given by sum of rs1 and imm_s to 16 LSBs of rs2.
            if(pos) 
            {
                *pos << "// m16(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_s) << ") = ";
                *pos << hex::to_hex0x32(rs2Con & 0x0000ffff) << std::endl;
            }
        }
        break;

        case funct3_sw:
        {
            mem.set32(addr, rs2Con); //Store 32-bit value in rs2 into memory at address given by sum of rs1 and imm_s.
            if(pos) 
            {
                *pos << "// m32(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_s) << ") = ";
                *pos << hex::to_hex0x32(rs2Con) << std::endl;
            }
        }
        break;
    }

    pc += 4;
}

void rv32i_hart::exec_itype_alu(uint32_t insn, std::ostream* pos, uint32_t funct3, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1Con = regs.get(get_rs1(insn)); //Contents of rs1.
    int32_t imm_i = get_imm_i(insn);
    int32_t val; //Value to set register.

    if(pos) //If output stream exists.
    {
        std::string s;
        if(funct3 == funct3_sll || funct3 == funct3_srx)
        {
            s = render_itype_alu(insn, mnemonic, imm_i%XLEN);
        }
        else
        {
            s = render_itype_alu(insn, mnemonic, imm_i);
        }

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
    }

    switch(funct3) //Discriminate further by funct3.
    {
        default:                exec_illegal_insn(insn, pos); return;
        case funct3_add:
        {
            val = rs1Con + imm_i; //Set register rd to rs1 + imm_i.
            if(pos) 
            {
                *pos << "// " << render_reg(rd) << " = " << hex :: to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << " = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_slt:     
        {
            val = ((static_cast<int32_t>(rs1Con) < imm_i) ? 1 : 0); //If signed integer value in rs1 is less than signed integer value    
            if(pos)                                                                       //in imm_i, then set rd to 1. Otherwise, set rd to 0. 
            {
                *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1Con) << " < " << imm_i;
                *pos << ") ? 1 : 0 = " << hex::to_hex0x32(val)<< std::endl;
            }
        }
        break;

        case funct3_sltu:        
        {
            val = ((rs1Con < static_cast<uint32_t>(imm_i)) ? 1 : 0); //If the unsigned integer value in rs1 is less than the unsigned integer value
            if(pos)                           //in imm_i, then set rd to 1. Otherwise, set rd to 0.                                                  
            {
                *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1Con) << " <U " << imm_i;
                *pos << ") ? 1 : 0 = " << hex::to_hex0x32(val)<< std::endl;
            }
        }
        break;

        case funct3_xor:     
        {
            val = (rs1Con ^ imm_i); //Set register rd to the bitwise xor of rs1 and imm_i.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " ^ " << hex::to_hex0x32(imm_i);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_or:      
        {
            val = (rs1Con | imm_i); //Set register rd to the bitwise or of rs1 and imm_i.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " | " << hex::to_hex0x32(imm_i);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_and:       
        {
            val = (rs1Con & imm_i); //Set register rd to the bitwise and of rs1 and imm_i.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " & " << hex::to_hex0x32(imm_i);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_sll:        
        {
            val = (rs1Con << imm_i%XLEN); //Shift rs1 left by the number of bits specifed in shamt_i and store the result in the rd register.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " << " << imm_i%XLEN;
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_srx:
        switch(get_funct7(insn)) //Discriminate further by funct7.
        {
            default:            
            case funct7_sra:
            {
                val = (static_cast<int32_t>(rs1Con) >> imm_i%XLEN); //Arithmetic shift rs1 right by the number of bits specifed in shamt_i 
                if(pos)                                             //and store the result in the rd register.
                {
                    *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " >> " << imm_i%XLEN;
                    *pos << " = " << hex::to_hex0x32(val) << std::endl;
                }
            }
            break;

            case funct7_srl:
            {
                val = (rs1Con >> imm_i%XLEN); //Logical shift rs1 right by the number of bits specifed in shamt_i and store the result in the rd register.
                if(pos)                                                     
                {
                    *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " >> " << imm_i%XLEN;
                    *pos << " = " << hex::to_hex0x32(val) << std::endl;
                }
            }
            break;    
        }
    }

    regs.set(rd , val);
    pc += 4;
}

/**
 * @brief Execute ebreak.
 * 
 * @param insn 
 * @param pos 
 */

/*
void rv32i_hart::exec_ebreak(uint32_t insn, std::ostream* pos)
{
    if(pos)
    {
        std::string s = render_ebreak(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// HALT";
    }

    halt = true;
    halt_reason = "EBREAK instruction";
} */