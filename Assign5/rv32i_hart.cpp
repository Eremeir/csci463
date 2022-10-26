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
    if(is_halted()) //Check status of hart before continuing execution.
    {   
        return;
    }
    
    if(show_registers) //Dump according to set flag.
    {
        dump(hdr);
    }

    if(!(pc % 4 == 0)) //Ensure memory is aligned to 4 byte multiple boundaries.
    {
        halt = true;
        halt_reason = "PC alignment error";
        return;
    }

    insn_counter++;

    uint32_t insn = mem.get32(pc); //Fetch instruction from memory.

    if(show_instructions) //Print insn according to set flag.
    {
        std::cout << hdr << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
        exec(insn, &std::cout);
    }
    else
    {
        exec(insn, nullptr); //Send any output to null.
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
    regs.dump(hdr);
    if(hdr != "") //If prefix string isn't blank.
        {
            std::cout << hdr;
        }
    std::cout << " pc " << to_hex32(pc) << std::endl;
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
    regs.reset(); //Reset the registers. 
    insn_counter = 0; //Reset hart status variables.
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
        case opcode_lui:        exec_lui(insn, pos); return;             //Load Upper Immediate
        case opcode_auipc:      exec_auipc(insn, pos); return;           //Add Upper Immediate to PC
        case opcode_jal:        exec_jal(insn, pos); return;             //Jump And Link
        case opcode_jalr:       exec_jalr(insn, pos); return;            //Jump And Link Register
        
        case opcode_btype:      
        switch(funct3) //Discriminate further by funct3.
        {
            default:            exec_illegal_insn(insn, pos); return;
            case funct3_beq:    exec_btype(insn, pos, funct3, "beq"); return;   //Branch Equal
            case funct3_bne:    exec_btype(insn, pos, funct3, "bne"); return;   //Branch Not Equal
            case funct3_blt:    exec_btype(insn, pos, funct3, "blt"); return;   //Branch Less Than
            case funct3_bge:    exec_btype(insn, pos, funct3, "bge"); return;   //Branch Greater or Equal
            case funct3_bltu:   exec_btype(insn, pos, funct3, "bltu"); return;  //Branch Less Than Unsigned    
            case funct3_bgeu:   exec_btype(insn, pos, funct3, "bgeu"); return;  //Branch Greater or Equal Unsigned
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!
        
        case opcode_load_imm:   
        switch(funct3) //Discriminate further by funct3.
        {
            default:                exec_illegal_insn(insn, pos); return;
            case funct3_lb:         exec_itype_load(insn, pos, funct3, "lb"); return;   //Load Byte
            case funct3_lh:         exec_itype_load(insn, pos, funct3, "lh"); return;   //Load Halfword
            case funct3_lw:         exec_itype_load(insn, pos, funct3, "lw"); return;   //Load Word
            case funct3_lbu:        exec_itype_load(insn, pos, funct3, "lbu"); return;  //Load Byte Unsigned
            case funct3_lhu:        exec_itype_load(insn, pos, funct3, "lhu"); return;  //Load Halfword Unsigned
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!
        
        case opcode_stype:       
        switch(funct3) //Discriminate further by funct3.
        {
            default:                exec_illegal_insn(insn, pos); return;
            case funct3_sb:         exec_stype(insn, pos, funct3, "sb"); return;  //Set Byte
            case funct3_sh:         exec_stype(insn, pos, funct3, "sh"); return;  //Set Halfword
            case funct3_sw:         exec_stype(insn, pos, funct3, "sw"); return;  //Set Word
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_alu_imm:       
        switch(funct3) //Discriminate further by funct3.
        {
            default:                exec_illegal_insn(insn, pos); return;
            case funct3_add:        exec_itype_alu(insn, pos, funct3, "addi"); return;   //Add Immediate
            case funct3_slt:        exec_itype_alu(insn, pos, funct3, "slti"); return;   //Set Less Than Immediate
            case funct3_sltu:       exec_itype_alu(insn, pos, funct3, "sltiu"); return;  //Set Less Than Immediate Unsigned
            case funct3_xor:        exec_itype_alu(insn, pos, funct3, "xori"); return;   //Exclusive Or Immediate
            case funct3_or:         exec_itype_alu(insn, pos, funct3, "ori"); return;    //Or Immediate
            case funct3_and:        exec_itype_alu(insn, pos, funct3, "andi"); return;   //And Immediate

            case funct3_sll:        exec_itype_alu(insn, pos, funct3, "slli"); return;   //Shift Left Logical Immediate
            case funct3_srx:
            switch(get_funct7(insn)) //Discriminate further by funct7.
            {
                default:            exec_illegal_insn(insn, pos); return;
                case funct7_sra:    exec_itype_alu(insn, pos, funct3, "srai"); return;  //Shift Right Arithmetic Immediate
                case funct7_srl:    exec_itype_alu(insn, pos, funct3, "srli"); return;  //Shift Right Logical Immediate
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!
        
        case opcode_rtype:
        switch(funct3) //Discriminate further by funct3.
        {
            default:                exec_illegal_insn(insn, pos); return;
            case funct3_sll:        exec_rtype(insn, pos, funct3, "sll"); return;    //Shift Left Logical
            case funct3_slt:        exec_rtype(insn, pos, funct3, "slt"); return;    //Set Less Than
            case funct3_sltu:       exec_rtype(insn, pos, funct3, "sltu"); return;   //Set Less Than Unsigned
            case funct3_xor:        exec_rtype(insn, pos, funct3, "xor"); return;    //Exclusive Or
            case funct3_or:         exec_rtype(insn, pos, funct3, "or"); return;     //Or
            case funct3_and:        exec_rtype(insn, pos, funct3, "and"); return;    //And

            case funct3_add:
            switch(get_funct7(insn))
            {
                default:            exec_illegal_insn(insn, pos); return;
                case funct7_add:    exec_rtype(insn, pos, funct3, "add"); return;    //Add
                case funct7_sub:    exec_rtype(insn, pos, funct3, "sub"); return;    //Subtract
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
            
            case funct3_srx:
            switch(get_funct7(insn)) //Discriminate further by funct7.
            {
                default:            exec_illegal_insn(insn, pos); return;
                case funct7_sra:    exec_rtype(insn, pos, funct3, "sra"); return;    //Shift Right Arithmetic
                case funct7_srl:    exec_rtype(insn, pos, funct3, "srl"); return;    //Shift Right Logical
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!
        
        case opcode_system:
        switch(funct3) //Discriminate further by funct3.
        {
            default:                exec_illegal_insn(insn, pos); return;
            case eCode:
            switch(insn) //Check if instruction matches system ecodes.            
            {
                default:            exec_illegal_insn(insn, pos); return;
                case insn_ecall:    exec_ecall(insn, pos); return;   //Trap to Debugger
                case insn_ebreak:   exec_ebreak(insn, pos); return;  //Trap to Operating System
            }
            assert(0 && "unrecognized code"); //It should be impossible to ever get here!

            case funct3_csrrw:      exec_csrrx(insn, pos, funct3, "csrrw"); return;    //Atomic Read/Write
            case funct3_csrrs:      exec_csrrx(insn, pos, funct3, "csrrs"); return;    //Atomic Read and Set
            case funct3_csrrc:      exec_csrrx(insn, pos, funct3, "csrrc"); return;    //Atomic Read and Clear

            case funct3_csrrwi:     exec_csrrxi(insn, pos, funct3, "csrrwi"); return;  //Atomic Read/Write Immediate
            case funct3_csrrsi:     exec_csrrxi(insn, pos, funct3, "csrrsi"); return;  //Atomic Read and Set
            case funct3_csrrci:     exec_csrrxi(insn, pos, funct3, "csrrci"); return;  //Atomic Read and Clear Immediate
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!
    }
    assert(0 && "unrecognized opcode"); //It should be impossible to ever get here!
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
        case funct3_beq:  //Branch Equal
        {
            val = ((rs1Con == rs2Con) ? imm_b : 4); //If rs1 is equal to rs2 then add imm_b to pc register, otherwise 4.
            if(pos) 
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " == " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val) << std::endl;
            }
        }
        break;

        case funct3_bne:  //Branch Not Equal
        {
            val = ((rs1Con != rs2Con) ? imm_b : 4); //If rs1 is not equal to rs2 then add imm_b to pc register, otherwise 4.
            if(pos) 
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " != " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val)<< std::endl;
            }
        }
        break;

        case funct3_blt:  //Branch Less Than
        {

            val = ((static_cast<int32_t>(rs1Con) < static_cast<int32_t>(rs2Con)) ? imm_b : 4); //If signed val in rs1 is less than signed val in rs2 
            if(pos)                                                                            //then add imm_b to pc register, otherwise 4.
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " < " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val) << std::endl;
            }
        }
        break;

        case funct3_bge:  //Branch Greater or Equal
        {
            val = ((static_cast<int32_t>(rs1Con) >= static_cast<int32_t>(rs2Con)) ? imm_b : 4); //If signed val in rs1 is greater than or equal to 
            if(pos)                                                                             //signed val in rs2 then add imm_b to pc register, otherwise 4.
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " >= " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val) << std::endl;
            }
        }
        break;

        case funct3_bltu:  //Branch Less Than Unsigned
        {
             val = ((rs1Con < rs2Con) ? imm_b : 4); //If unsigned val in rs1 is less than unsigned val in rs2 then add imm_b to pc register, otherwise 4.
            if(pos)
            {
                *pos << "// pc += (" << hex::to_hex0x32(rs1Con) << " <U " << hex::to_hex0x32(rs2Con) << " ? ";
                *pos << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(pc + val) << std::endl;
            }
        }
        break;

        case funct3_bgeu:  //Branch Greater or Equal Unsigned
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
        case funct3_lb:   //Load Byte
        {
            val = mem.get8_sx(rs1Con + imm_i); //Set register rd to value of sign-extended byte fetched from memory address given by sum of rs1 and imm_i.
            if(pos)
            {
                *pos << "// " << render_reg(rd) << " = sx(m8(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << ")) = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_lh:   //Load Halfword
        {
            val = mem.get16_sx(rs1Con + imm_i); //Set register rd to value of sign-extended 16-bit little-endian half-word value
            if(pos)                             //from memory address given by sum of rs1 and imm_i.
            {
                *pos << "// " << render_reg(rd) << " = sx(m16(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << ")) = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_lw:   //Load Word
        {
            val = mem.get32_sx(rs1Con + imm_i); //Set register rd to value of sign-extended 32-bit little-endian word value
            if(pos)                             //from memory address given by sum of rs1 and imm_i.
            {
                *pos << "// " << render_reg(rd) << " = sx(m32(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << ")) = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_lbu:  //Load Byte Unsigned
        {
            val = mem.get8(rs1Con + imm_i); //Set register rd to value of zero-extended byte from memory address given by sum of rs1 and imm_i.
            if(pos)
            {
                *pos << "// " << render_reg(rd) << " = zx(m8(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << ")) = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_lhu:   //Load Halfword Unsigned
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
        case funct3_sb:  //Set Byte
        {
            mem.set8(addr, rs2Con & 0x000000ff); //Set byte of memory at address given by sum of rs1 and imm_s to 8 LSBs of rs2.
            if(pos)
            {
                *pos << "// m8(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_s) << ") = ";
                *pos << hex::to_hex0x32(rs2Con & 0x000000ff) << std::endl;
            }
        }
        break;

        case funct3_sh:  //Set Halfword
        {
            mem.set16(addr, rs2Con& 0x0000ffff); //Set 16-bit half-word of memory at address given by sum of rs1 and imm_s to 16 LSBs of rs2.
            if(pos) 
            {
                *pos << "// m16(" << hex::to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_s) << ") = ";
                *pos << hex::to_hex0x32(rs2Con & 0x0000ffff) << std::endl;
            }
        }
        break;

        case funct3_sw:  //Set Word
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

/**
 * @brief Execute I Type-ALU instruction.
 * 
 * Execute between several I Type-ALU instructions based on funct3 code.
 *
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 * @param funct3 Value to discriminate I Type-ALU instructions.
 * @param mnemonic String to pass to rendering function.
 */
void rv32i_hart::exec_itype_alu(uint32_t insn, std::ostream* pos, uint32_t funct3, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1Con = regs.get(get_rs1(insn)); //Contents of rs1.
    int32_t imm_i = get_imm_i(insn);
    int32_t val; //Value to set register.

    if(pos) //If output stream exists.
    {
        std::string s;
        if(funct3 == funct3_sll || funct3 == funct3_srx) //If funct3 = operation with shamt requirement.
        {
            s = render_itype_alu(insn, mnemonic, imm_i%XLEN);
        }
        else
        {
            s = render_itype_alu(insn, mnemonic, imm_i);
        }

        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
    }

    switch(funct3) //Discriminate by funct3.
    {
        default:                exec_illegal_insn(insn, pos); return;
        case funct3_add:  //Add Immediate
        {
            val = rs1Con + imm_i; //Set register rd to rs1 + imm_i.
            if(pos) 
            {
                *pos << "// " << render_reg(rd) << " = " << hex :: to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(imm_i) << " = ";
                *pos << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_slt:  //Set Less Than Immediate 
        {
            val = ((static_cast<int32_t>(rs1Con) < imm_i) ? 1 : 0); //If signed integer value in rs1 is less than signed integer value    
            if(pos)                                                 //in imm_i, then set rd to 1. Otherwise, set rd to 0. 
            {
                *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1Con) << " < " << imm_i;
                *pos << ") ? 1 : 0 = " << hex::to_hex0x32(val)<< std::endl;
            }
        }
        break;

        case funct3_sltu:  //Set Less Than Immediate Unsigned    
        {
            val = ((rs1Con < static_cast<uint32_t>(imm_i)) ? 1 : 0); //If the unsigned integer value in rs1 is less than the unsigned integer value
            if(pos)                                                  //in imm_i, then set rd to 1. Otherwise, set rd to 0.                                                  
            {
                *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1Con) << " <U " << imm_i;
                *pos << ") ? 1 : 0 = " << hex::to_hex0x32(val)<< std::endl;
            }
        }
        break;

        case funct3_xor:  //Exclusive Or Immediate
        {
            val = (rs1Con ^ imm_i); //Set register rd to the bitwise xor of rs1 and imm_i.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " ^ " << hex::to_hex0x32(imm_i);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_or:   //Or Immediate
        {
            val = (rs1Con | imm_i); //Set register rd to the bitwise or of rs1 and imm_i.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " | " << hex::to_hex0x32(imm_i);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_and:  //And Immediate
        {
            val = (rs1Con & imm_i); //Set register rd to the bitwise and of rs1 and imm_i.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " & " << hex::to_hex0x32(imm_i);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_sll:  //Shift Left Logical Immediate       
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
            case funct7_sra:  //Shift Right Arithmetic Immediate
            {
                val = (static_cast<int32_t>(rs1Con) >> imm_i%XLEN); //Arithmetic shift rs1 right by the number of bits specifed in shamt_i 
                if(pos)                                             //and store the result in the rd register.
                {
                    *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " >> " << imm_i%XLEN;
                    *pos << " = " << hex::to_hex0x32(val) << std::endl;
                }
            }
            break;

            case funct7_srl:  //Shift Right Logical Immediate
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
 * @brief Execute R instruction.
 * 
 * Execute between several R Type instructions based on funct3 code.
 *
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 * @param funct3 Value to discriminate R Type instructions.
 * @param mnemonic String to pass to rendering function.
 */
void rv32i_hart::exec_rtype(uint32_t insn, std::ostream* pos, uint32_t funct3, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1Con = regs.get(get_rs1(insn)); //Contents of rs1.
    uint32_t rs2Con = regs.get(get_rs2(insn)); //Contents of rs1.
    int32_t val; //Value to set register.

    if(pos) //If output stream exists.
    {
        std::string s = render_rtype(insn, mnemonic);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
    }

    switch(funct3) //Discriminate by funct3.
    {
        default:                exec_illegal_insn(insn, pos); return;    
        case funct3_sll:  //Shift Left Logical
        {
            val = (rs1Con << (rs2Con & 0x0000001f)); //Shift rs1 left by number of bits in least signifcant 5 bits of rs2 and store result in rd.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " << " << (rs2Con & 0x0000001f);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }     
        break;

        case funct3_slt:  //Set Less Than
        {
            val = ((static_cast<int32_t>(rs1Con) < static_cast<int32_t>(rs2Con)) ? 1 : 0); //If signed integer value in rs1 is less than signed integer value    
            if(pos)                                                                        //in rs2, then set rd to 1. Otherwise, set rd to 0. 
            {
                *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1Con) << " < " << hex::to_hex0x32(rs2Con);
                *pos << ") ? 1 : 0 = " << hex::to_hex0x32(val)<< std::endl;
            }
        }     
        break;

        case funct3_sltu:  //Set Less Than Unsigned
        {
            val = ((rs1Con < rs2Con) ? 1 : 0); //If unsigned integer value in rs1 is less than unsigned integer value    
            if(pos)                            //in rs2, then set rd to 1. Otherwise, set rd to 0. 
            {
                *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(rs1Con) << " <U " << hex::to_hex0x32(rs2Con);
                *pos << ") ? 1 : 0 = " << hex::to_hex0x32(val)<< std::endl;
            }
        }     
        break;

        case funct3_xor:  //Exclusive Or
        {
            val = (rs1Con ^ rs2Con); //Set register rd to the bitwise xor of rs1 and rs2.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " ^ " << hex::to_hex0x32(rs2Con);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        } 
        break;

        case funct3_or:  //OR
        {
            val = (rs1Con | rs2Con); //Set register rd to the bitwise or of rs1 and rs2.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " | " << hex::to_hex0x32(rs2Con);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_and:  //And    
        {
            val = (rs1Con & rs2Con); //Set register rd to the bitwise and of rs1 and rs2.
            if(pos)                                                     
            {
                *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " & " << hex::to_hex0x32(rs2Con);
                *pos << " = " << hex::to_hex0x32(val) << std::endl;
            }
        }
        break;

        case funct3_add:    //std::cout << "entering funcADD" << std::endl;
        switch(get_funct7(insn))
        {
            default:            exec_illegal_insn(insn, pos); return;
            case funct7_add:  //Add 
            {
                val = rs1Con + rs2Con; //Set register rd to rs1 + rs2.
                if(pos) 
                {
                    *pos << "// " << render_reg(rd) << " = " << hex :: to_hex0x32(rs1Con) << " + " << hex::to_hex0x32(rs2Con) << " = ";
                    *pos << hex::to_hex0x32(val) << std::endl;
                }
            }     
            break;

            case funct7_sub:  //Subtract
            {
                val = rs1Con - rs2Con; //Set register rd to rs1 - rs2.
                if(pos) 
                {
                    *pos << "// " << render_reg(rd) << " = " << hex :: to_hex0x32(rs1Con) << " - " << hex::to_hex0x32(rs2Con) << " = ";
                    *pos << hex::to_hex0x32(val) << std::endl;
                }
            }     
            break;
        }
        break;
        
        case funct3_srx:    //std::cout << "entering funcSRX" << std::endl;
        switch(get_funct7(insn)) //Discriminate further by funct7.
        {
            default:            exec_illegal_insn(insn, pos); return;
            case funct7_sra:  //Shift Right Arithmetic
            {
                val = (static_cast<int32_t>(rs1Con) >> (rs2Con & 0x0000001f)); //Arithmetic shift rs1 right by the number of bits given in the least-signifcant 5 bits 
                if(pos)                                                        //of the rs2 register and store the result in rd.
                {
                    *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " >> " << (rs2Con & 0x0000001f);
                    *pos << " = " << hex::to_hex0x32(val) << std::endl;
                }
            }   
            break;

            case funct7_srl:  //Shift Right Logical
            {
                val = (rs1Con >> (rs2Con & 0x0000001f)); //Logical shift rs1 right by the number of bits given in the least-signigcant 5 bits 
                if(pos)                                  //of the rs2 register and store the result in rd.   
                {
                    *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(rs1Con) << " >> " << (rs2Con & 0x0000001f);
                    *pos << " = " << hex::to_hex0x32(val) << std::endl;
                }
            }   
            break;
        }
        break;
    }

    regs.set(rd , val);
    pc += 4;
}

/**
 * @brief Execute ecall.
 *
 * Terminate and transfer back control to operating system, halting thread.
 * 
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 */
void rv32i_hart::exec_ecall(uint32_t insn, std::ostream* pos)
{
    if(pos) //If output stream exists.
    {
        std::string s = render_ecall(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// HALT";
    }

    halt = true;
    halt_reason = "ECALL instruction";
}

/**
 * @brief Execute ebreak.
 *
 * Terminate and transfer back control to debugger, halting thread.
 * 
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 */
void rv32i_hart::exec_ebreak(uint32_t insn, std::ostream* pos)
{
    if(pos) //If output stream exists.
    {
        std::string s = render_ebreak(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// HALT" << std::endl;
    }

    halt = true;
    halt_reason = "EBREAK instruction";
}

/**
 * @brief Execute csrrx instruction.
 * 
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 * @param funct3 Value to discriminate csrrx type instructions.
 * @param mnemonic String to pass to rendering function.
 */
void rv32i_hart::exec_csrrx(uint32_t insn, std::ostream* pos, uint32_t funct3, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = (get_rs1(insn)); 
    int32_t csr = get_imm_i(insn);
    int32_t val = 0; //Value to set register.

    if(pos) //If output stream exists.
    {
        std::string s = render_csrrx(insn, mnemonic);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
    }

    switch(funct3)
    {
        default:            exec_illegal_insn(insn, pos); return;
        case funct3_csrrw:  //Atomic Read/Write
        {
            if(rd != 0)
            {
                val = regs.get(csr);
            }

            regs.set(csr, regs.get(rs1));
            if(pos)
            {
                *pos << "// " << render_reg(rd) << " = " << render_reg(rs1) << std::endl;
            }
        }
        break;

        case funct3_csrrs:  //Atomic Read and Set
        {
            if((csr & 0x00000fff) == 0xf14)
            {
                val = mhartid;
            }
            else
            {
                if(csr > 31) //If invalid register requested.
                {
                    halt = true;
                    halt_reason = "Illegal CSR in CRRSS instruction";
                    return;
                }
                val = regs.get(csr);
            }

            if(rs1 != 0)
            {
                regs.set(csr, (val | regs.get(rs1)));
            }
            
            if(pos) 
            {
                *pos << "// " << render_reg(rd) << " = " << regs.get(rs1) << std::endl;
            }
        } 
        break;

        case funct3_csrrc:  //Atomic Read and Clear
        {
            val = regs.get(csr);
            if(rs1 != 0)
            {
                regs.set(csr, (val & ~regs.get(rs1)));
            }
            
            if(pos) 
            {
                *pos << "// " << render_reg(rd) << " = " << render_reg(rs1) << std::endl;
            }
        }
        break;
    }

    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief Execute csrrxi instruction.
 * 
 * @param insn Instruction to decode and execute.
 * @param pos Pointer to the output stream (if it exists) to send output.
 * @param funct3 Value to discriminate csrrxi type instructions.
 * @param mnemonic String to pass to rendering function.
 */
void rv32i_hart::exec_csrrxi(uint32_t insn, std::ostream* pos, uint32_t funct3, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t zimm = (get_rs1(insn));
    int32_t csr = get_imm_i(insn);
    int32_t val = 0; //Value to set register.

    if(pos) //If output stream exists.
    {
        std::string s = render_csrrxi(insn, mnemonic);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
    }

    switch(funct3)
    {
        default:            exec_illegal_insn(insn, pos); return;
        case funct3_csrrw:  //Atomic Read/Write Immediate
        {
            if(rd != 0)
            {
                val = regs.get(csr);
            }

            regs.set(csr, regs.get(zimm));
            if(pos)
            {
                *pos << "// " << render_reg(rd) << " = " << zimm << std::endl;
            }
        }
        break;

        case funct3_csrrs:  //Atomic Read and Set Immediate
        {
            val = regs.get(csr);
            if(zimm != 0)
            {
                regs.set(csr, (val | regs.get(zimm)));
            }
            
            if(pos) 
            {
                *pos << "// " << render_reg(rd) << " = " << zimm << std::endl;
            }
        }
        break;

        case funct3_csrrc:  //Atomic Read and Clear Immediate
        {
            val = regs.get(csr);
            if(zimm != 0)
            {
                regs.set(csr, (val & ~regs.get(zimm)));
            }
            
            if(pos) 
            {
                *pos << "// " << render_reg(rd) << " = " << zimm << std::endl;
            }
        }
        break;
    }

    regs.set(rd, val);
    pc += 4;
}