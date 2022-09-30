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
#include <iomanip>
#include <cassert>
#include "rv32i_decode.h"

/**
 * @brief Decode memory address instruction.
 * 
 * Decode an instruction set from memory and print mnemonic and list of operands.
 * 
 * @param addr The memory address where the insn is stored.
 * @param insn The instruction to decode.
 * @return std::string 
 */
std::string rv32i_decode::decode(uint32_t addr, uint32_t insn)
{
    switch(get_opcode(insn)) //Render based on determined opcode.
    {
        default:                return render_illegal_insn();
        case opcode_lui:        return render_lui(insn);
        case opcode_auipc:      return render_auipc(insn);
        case opcode_jal:        return render_jal(addr, insn);
        case opcode_jalr:       return render_jalr(insn);

        case opcode_btype:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:            return render_illegal_insn();
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
            default:                return render_illegal_insn();
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
            default:                return render_illegal_insn();
            case funct3_sb:         return render_stype(insn, "sb");
            case funct3_sh:         return render_stype(insn, "sh");
            case funct3_sw:         return render_stype(insn, "sw");
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_alu_imm:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:                return render_illegal_insn();
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
                default:            return render_illegal_insn();
                case funct7_sra:    return render_itype_alu(insn, "srai", get_imm_i(insn)%XLEN);
                case funct7_srl:    return render_itype_alu(insn, "srli", get_imm_i(insn)%XLEN);
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_rtype:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:                return render_illegal_insn();
            case funct3_sll:        return render_rtype(insn, "sll");
            case funct3_slt:        return render_rtype(insn, "slt");
            case funct3_sltu:       return render_rtype(insn, "sltu");
            case funct3_xor:        return render_rtype(insn, "xor");
            case funct3_or:         return render_rtype(insn, "or");
            case funct3_and:        return render_rtype(insn, "and");

            case funct3_add:
            switch(get_funct7(insn))
            {
                default:            return render_illegal_insn();
                case funct7_add:    return render_rtype(insn, "add");
                case funct7_sub:    return render_rtype(insn, "sub");
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
            
            case funct3_srx:
            switch(get_funct7(insn)) //Discriminate further by funct7.
            {
                default:            return render_illegal_insn();
                case funct7_sra:    return render_rtype(insn, "sra");
                case funct7_srl:    return render_rtype(insn, "srl");
            }
            assert(0 && "unrecognized funct7 code"); //It should be impossible to ever get here!
        }
        assert(0 && "unrecognized funct3 code"); //It should be impossible to ever get here!

        case opcode_system:
        switch(get_funct3(insn)) //Discriminate further by funct3.
        {
            default:                return render_illegal_insn();
            
            case eCode:
            switch(insn) //Check if instruction matches system ecodes.            
            {
                default:            return render_illegal_insn();
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
    assert(0 && "unrecognized opcode"); //It should be impossible to ever get here!
}

/**
 * @brief Retrieve instruction opcode.
 * 
 * Isolate and return opcode bitvalue from instruction.
 * 
 * @param insn Instruction to select opcode from.
 * @return uint32_t representing opcode for instruction or instruction set.
 */
uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
    return insn & 0x0000007f; //Select last 7 bits.
}

/**
 * @brief Retrieve result xregister.
 * 
 * Isolate and return rd bitvalue from instruction.
 * 
 * @param insn Instruction to select funct7 from.
 * @return uint32_t representing the rd result xregister.
 */
uint32_t rv32i_decode::get_rd(uint32_t insn)
{
    return (insn & 0x00000f80) >> 7; //Select 5 bits from rd, shift full right.
}

/**
 * @brief Retrieve funct3 discriminator.
 * 
 * Isolate and return funct3 bitvalue from instruction.
 * 
 * @param insn Instruction to select funct3 from.
 * @return uint32_t representing funct3 bitvalue. 
 */
uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
    return (insn & 0x00007000) >> 12; //Select 3 bits from funct3, shift full right.
}

/**
 * @brief Retrieve first source operand xregister.
 * 
 * Isolate and return rs1 bitvalue from instruction.
 * 
 * @param insn Instruction to select rs1 from.
 * @return uint32_t representing rs1 source operand xregister.  
 */
uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
    return (insn & 0x000f8000) >> 15; //Select 5 bits from rs1, shift full right.
}

/**
 * @brief Retrieve second source operand xregister.
 * 
 * Isolate and return rs2 bitvalue from instruction.
 * 
 * @param insn Instruction to select rs2 from.
 * @return uint32_t representing rs2 source operand xregister. 
 */
uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
    return (insn & 0x01f00000) >> 20; //Select 5 bits from rs2, shift full right.
}

/**
 * @brief Retrieve funct7 discriminator.
 * 
 * Isolate and return funct7 bitvalue from instruction.
 * 
 * @param insn Instruction to select funct7 from.
 * @return iint32_t representing funct7 bitvalue.  
 */
uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
    return (insn & 0xfe000000) >> 25; //Select first 7 bits, shift full right.
}

/**
 * @brief Retrieve immediate numeric operand. (I Type)
 * 
 * Isolate and return IMM_I bitvalue from instruction.
 * 
 * @param insn Instruction to select IMM from.
 * @return int32_t representing IMM_I immediate numeric operand. 
 */
int32_t rv32i_decode::get_imm_i(uint32_t insn)
{
    int32_t imm_i = insn;
    return imm_i >> 20; //Arith shift full to right.
}

/**
 * @brief Retrieve immediate numeric operand. (U Type)
 * 
 * Isolate and return IMM_U bitvalue from instruction.
 * 
 * @param insn Instruction to select IMM from.
 * @return int32_t representing IMM_U immediate numeric operand. 
 */
int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
    int32_t imm_u = insn & 0xfffff000; //Set to first 20 bits.
    return imm_u;
}

/**
 * @brief Retrieve immediate numeric operand. (B Type)
 * 
 * Isolate and return IMM_B bitvalue from instruction.
 * 
 * @param insn Instruction to select IMM from.
 * @return int32_t representing IMM_B immediate numeric operand. 
 */
int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
    int32_t imm_b = insn & 0x80000000; //Set to first bit.
    imm_b >>= (31-12); //Arith shift right 19.
    imm_b |= (insn & 0x7e000000) >> (25-5); //Select next 6 bits, place 20 to right.
    imm_b |= (insn & 0x00000f00) >> (8-1); //Select 4 bits, place 7 to right.
    imm_b |= (insn & 0x00000080) << (11-7); //Select final relevent bit, place 4 to left.

    return imm_b;
}

/**
 * @brief Retrieve immediate numeric operand. (S Type)
 * 
 * Isolate and return IMM_S bitvalue from instruction.
 * 
 * @param insn Instruction to select IMM from.
 * @return int32_t representing IMM_S immediate numeric operand. 
 */
int32_t rv32i_decode::get_imm_s(uint32_t insn)
{
    int32_t imm_s = insn & 0xfe000000; //Set to first 7 bits.
    imm_s >>= (25-5); //Arith shift right 20.
    imm_s |= (insn & 0x00000f80) >> (7-0); //Select 5 bits, place 7 to right.

    return imm_s;
}

/**
 * @brief Retrieve immediate numeric operand. (J Type)
 * 
 * Isolate and return IMM_J bitvalue from instruction.
 * 
 * @param insn Instruction to select IMM from.
 * @return int32_t representing IMM_J immediate numeric operand. 
 */
int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
    
    int32_t imm_j = insn & 0x80000000; //Set to first bit.
    imm_j >>= (31-20); //Arith shift right 11.
    imm_j |= (insn & 0x000ff000); //Pull down 8 bits.
    imm_j |= (insn & 0x00100000) >> (20-11); //Select bit and place 9 to right.
    imm_j |= (insn & 0x7fe00000) >> (21-1); //Select remaining bits and place 20 to right.

    return imm_j;
}

/**
 * @brief Render illegal instruction message.
 * 
 * Indicate INSN did not match any implimented function and may be invalid.
 * 
 * @return string indicating INSN was not recognized.
 */
std::string rv32i_decode::render_illegal_insn()
{
    return "ERROR: UNIMPLEMENTED INSTRUCTION";
}

/**
 * @brief Render lui instruction message.
 * 
 * Decode and render lui instruction in rd,imm format.
 * 
 * @param insn Instruction to decode and render.
 * @return string of lui instruction formatting. 
 */
std::string rv32i_decode::render_lui(uint32_t insn)
{
    std::ostringstream os;
    os << render_mnemonic("lui") << render_reg(get_rd(insn)) << ","
       << hex::to_hex0x20((get_imm_u(insn) >> 12) & 0x0fffff);
    return os.str();
}

/**
 * @brief Render auipc instruction message.
 * 
 * Decode and render auipc instruction in rd,imm format.
 * 
 * @param insn Instruction to decode and render.
 * @return string of auipc instruction formatting. 
 */
std::string rv32i_decode::render_auipc(uint32_t insn)
{
    std::ostringstream os;
    os << render_mnemonic("auipc") << render_reg(get_rd(insn)) << ","
       << hex::to_hex0x20((get_imm_u(insn) >> 12) & 0x0fffff);
    return os.str();
}

/**
 * @brief Render jal instruction message.
 * 
 * Decode and render jal instruction in rd,pcrel_21 format.
 * 
 * @param addr The memory address where the insn is stored.
 * @param insn Instruction to decode and render.
 * @return string of jal instruction formatting.
 */
std::string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
    std::ostringstream os;
    os << render_mnemonic("jal") << render_reg(get_rd(insn)) << ","
       << hex::to_hex0x32((get_imm_j(insn) + addr));
    return os.str();
}

/**
 * @brief Render jalr instruction message.
 * 
 * Decode and render jalr instruction in rd,imm(rs1) format.
 * 
 * @param insn Instruction to decode and render.
 * @return string of jalr instruction formatting.
 */
std::string rv32i_decode::render_jalr(uint32_t insn)
{
    std::ostringstream os;
    os << render_mnemonic("jalr") << render_reg(get_rd(insn)) << ","
       << render_base_disp(get_rs1(insn), get_imm_i(insn));
    return os.str();
}

/**
 * @brief Render B Type instruction.
 * 
 * Decode and render B type instructions in rs1,rs2,pcrel_13 format.
 * 
 * @param addr The memory address where the insn is stored.
 * @param insn Instruction to decode and render.
 * @param mnemonic of instruction to be rendered.
 * @return string of B Type instruction set formatting.
 */
std::string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char *mnemonic)
{
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(get_rs1(insn)) << "," << render_reg(get_rs2(insn)) << ","
       << hex::to_hex0x32((get_imm_b(insn) + addr));
    return os.str();
}

/**
 * @brief Render I Type-LOAD instruction.
 * 
 * Decode and render I Type-LOAD instructions in rd,imm(rs1) format.
 * 
 * @param insn Instruction to decode and render.
 * @param mnemonic of instruction to be rendered.
 * @return string of I Type-LOAD instruction set formatting.
 */
std::string rv32i_decode::render_itype_load(uint32_t insn, const char *mnemonic)
{
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << ","
       << render_base_disp(get_rs1(insn), get_imm_i(insn));
    return os.str();
}

/**
 * @brief Render S Type instruction.
 * 
 * Decode and render S type instructions in rs2,imm(rs1) format.
 * 
 * @param insn Instruction to decode and render.
 * @param mnemonic of instruction to be rendered.
 * @return string of S Type instruction set formatting.
 */
std::string rv32i_decode::render_stype(uint32_t insn, const char *mnemonic)
{
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(get_rs2(insn)) << ","
       << render_base_disp(get_rs1(insn), get_imm_s(insn));
    return os.str();
}

/**
 * @brief Render I Type-ALU instruction.
 * 
 * Decode and render I Type-ALU instructions in rd,rs1,imm or rd,rs1,shamt format. 
 * 
 * @param insn Instruction to decode and render.
 * @param mnemonic of instruction to be rendered.
 * @param imm_i imm_i portion of instruction. Used to substitute shamt portion by %XLEN in funct call.
 * @return string of I Type-ALU instruction set formatting.
 */
std::string rv32i_decode::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i)
{
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << ","
       << render_reg(get_rs1(insn)) << "," << imm_i;
    return os.str();
}

/**
 * @brief Render R Type instruction.
 * 
 * Decode and render R type instructions in rd,rs1,rs2 format.
 * 
 * @param insn Instruction to decode and render.
 * @param mnemonic of instruction to be rendered.
 * @return string of R Type instruction set formatting.
 */
std::string rv32i_decode::render_rtype(uint32_t insn, const char *mnemonic)
{
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << ","
       << render_reg(get_rs1(insn)) << "," << render_reg(get_rs2(insn));
    return os.str();
}

/**
 * @brief Render ecall instruction message.
 * 
 * Render ecall instruction without standard mnemonic formatting.
 * 
 * @param insn Instruction to decode and render.
 * @return string of ecall instruction formatting.
 */
std::string rv32i_decode::render_ecall(uint32_t insn)
{
    (void)insn; //Insn not used in function, but part of standard spec.
    return "ecall"; //Returns mnemonic directly instead of calling render func.
}

/**
 * @brief Render ebreak instruction message.
 * 
 * Render ebreak instruction without standard mnemonic formatting.
 * 
 * @param insn Instruction to decode and render.
 * @return string of ebreak instruction formatting.
 */
std::string rv32i_decode::render_ebreak(uint32_t insn)
{
    (void)insn; //Insn not used in function, but part of standard spec.
    return "ebreak"; //Returns mnemonic directly instead of calling render func.
}

/**
 * @brief Render csrrx instruction set message.
 * 
 * Decode and render csrrx type system instructions in rd,csr,rs1 format.
 * 
 * @param insn Instruction to decode and render.
 * @param mnemonic of instruction to be rendered.
 * @return string of csrrx instruction set formatting.
 */
std::string rv32i_decode::render_csrrx(uint32_t insn, const char *mnemonic)
{
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << ","
       << hex::to_hex0x12((get_imm_i(insn))) << "," << render_reg(get_rs1(insn));
    return os.str();
}

/**
 * @brief Render csrrxi instruction set message.
 * 
 * Decode and render csrrxi type system instructions in rd,csr,zimm format.
 * 
 * @param insn Instruction to decode and render.
 * @param mnemonic of instruction to be rendered.
 * @return string of csrrxi instruction set formatting.
 */
std::string rv32i_decode::render_csrrxi(uint32_t insn, const char *mnemonic)
{
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << ","
       << hex::to_hex0x12((get_imm_i(insn))) << "," << get_rs1(insn);
    return os.str();
}

/**
 * @brief Render xregister formatting.
 * 
 * Render x(register) style parameter.
 * 
 * @param reg xregister to render.
 * @return string of x(register) formatting.
 */
std::string rv32i_decode::render_reg(int reg)
{
    std::ostringstream os;
    os << "x" << std::dec << reg;
    return os.str();
}

/**
 * @brief Render displacement off of base formatting.
 * 
 * Render displacement(Base) style parameter.
 * 
 * @param base Source operand xregister to use as base address.
 * @param disp Immediate numeric operand to displace off of base.
 * @return string of displacement(Base) formatting.
 */
std::string rv32i_decode::render_base_disp(uint32_t base, int32_t disp)
{
    std::ostringstream os;
    os << disp << "(" << render_reg(base) << ")";
    return os.str();
}

/**
 * @brief Render mnemonic formatting.
 * 
 * Add mnemonic string to output with standardized width spacing.
 * 
 * @param mnemonic string to add to output.
 * @return string of mnemonic with standard width spacing.
 */
std::string rv32i_decode::render_mnemonic(const std::string &mnemonic)
{
    std::ostringstream os;
    os << std::left << std::setfill(' ') << std::setw(8) << mnemonic;
    return os.str();
}