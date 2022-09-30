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
#include <fstream>
#include <string>
#include "memory.h"

/**
 * @brief Construct a new memory vector.
 * 
 * Create memory object and resize to rounded value. Initializes to 0xa5.
 * 
 * @param size Given memory size. Will be rounded up to nearest 16th byte.
 */
memory::memory(uint32_t size)
{
    size = (size+15)&0xfffffff0; //round the length up, mod-16.
    mem.resize(size, 0xa5);
}

/**
 * @brief Destroy the memory vector.
 * 
 * @note Redundant, done automatically at end of scope.
 * 
 */
memory::~memory()
{

}

/**
 * @brief Check index validity.
 * 
 * Test index validity and print warning messages to cout.
 * 
 * @param addr Index address to test.
 * @return true if the index is beyond the size of the simulated memory.
 * @return false if the index is within the size of the simulated memory.
 */
bool memory::check_illegal(uint32_t addr) const
{
    if(addr < mem.size())
    {
        return false;
    }
    else
    {
        std::cout << "WARNING: Address out of range: " << hex::to_hex0x32(addr) << std::endl;
        return true;
    }
}

/**
 * @brief Get memory size.
 * 
 * @return 32bit integer for size of simulated memory index.
 */
uint32_t memory::get_size() const
{
    return mem.size();
}

/**
 * @brief Get 8bits of memory.
 * 
 * Check validity of index and return contents of memory at specified index address.
 * 
 * @param addr Index address to access.
 * @return 8bit integer for contents of address byte. Returns 0 in case of invalid address.
 */
uint8_t memory::get8(uint32_t addr) const
{
    if(check_illegal(addr)) //Check validity of index before accessing.
    {
        return 0;
    }
    else
    {
        return mem[addr];
    }
}

/**
 * @brief Get 16bits of memory.
 * 
 * Call get8() twice to retrieve two bytes then concatenate in little endian order.
 *
 * @param addr Index address to access.
 * @return 16bit integer for contents of address bytes.
 */
uint16_t memory::get16(uint32_t addr) const
{
    return get8(addr) | (static_cast<uint16_t>(get8(addr+1)) << 8);
}

/**
 * @brief Get 32bits of memory.
 * 
 * Call get16() twice to retrieve four bytes then concatenate in little endian order.
 * 
 * @param addr Index address to access.
 * @return 32bit integer for contents of address bytes.
 */
uint32_t memory::get32(uint32_t addr) const
{
    return get16(addr) | (static_cast<uint32_t>(get16(addr+2)) << 16);;
}

/**
 * @brief Get 32bits of 8 bit sign extended memory. 
 * 
 * Logically shift 8 bits to the boundary of 32bit storage then arithmetically shift back to sign extend. 
 * 
 * @param addr Index address to access.
 * @return Signed 32bit integer for contents of address bytes.
 */
int32_t memory::get8_sx(uint32_t addr) const
{
    int32_t tempInt = get8(addr) << 24; //Logical shift left.
    return tempInt >> 24;               //Arithmetic shift right.
}

/**
 * @brief Get 32bits of 16 bit sign extended memory. 
 * 
 * Logically shift 16 bits to the boundary of 32bit storage then arithmetically shift back to sign extend. 
 * 
 * @param addr Index address to access.
 * @return Signed 32bit integer for contents of address bytes.
 */
int32_t memory::get16_sx(uint32_t addr) const
{
    int32_t tempInt = get16(addr) << 16; //Logical shift left.
    return tempInt >> 16;                //Arithmetic shift right.
}

/**
 * @brief Get 32bits of signed memory.
 * 
 * Reformat 32 bits as signed storage.
 * 
 * @param addr Index address to access.
 * @return Signed 32bit integer for contents of address bytes.
 */
int32_t memory::get32_sx(uint32_t addr) const
{
    return get32(addr); //Automatically casts through return type.
}

/**
 * @brief Set 8bits of memory.
 * 
 * Check validity of address when write to simulated memory.
 * 
 * @param addr Index address to write to.
 * @param val 8bit value to write to memory.
 */
void memory::set8(uint32_t addr, uint8_t val)
{
    if(!check_illegal(addr)) //Check validity of index before accessing.
    {
        mem[addr] = val;
    } 
}

/**
 * @brief Set 16bits of memory.
 * 
 * Logically shift left and right to isolate target bytes then call set8() to write values.
 * 
 * @param addr Index address to write to.
 * @param val 16bit value to write to memory.
 */
void memory::set16(uint32_t addr, uint16_t val)
{
    set8(addr+1, val >> 8); //Shift right to cut off right byte.
    set8(addr, (val << 8) >> 8); //Shift left then right to cut off left byte.
}

/**
 * @brief Set 32bits of memory.
 * 
 * Logically shift left and right to isolate target bytes then call set16() to write values.
 * 
 * @param addr Index address to write to.
 * @param val 32bit value to write to memory.
 */
void memory::set32(uint32_t addr, uint32_t val)
{
    set16(addr+2, val >> 16); //Shift right to cut off right bytes.
    set16(addr, (val << 16) >> 16); //Shift left then right to cut off left bytes.
}

/**
 * @brief Print memory dump.
 * 
 * Print contents of memory to human-readable text output.
 * 
 */
void memory::dump() const
{
    uint32_t printCount = 0; //Number of bytes printed.
    for(uint32_t i = 0; i < get_size() / 16; ++i) //Number of printlines is the total memory size divided into 16 byte lines.
    {
        std::cout << hex::to_hex32(printCount) << ": ";
        for(uint32_t j = 0; j < 16; ++j) //Print memory byte by byte for current line.
        {
            std::cout << hex::to_hex8(get8(printCount)) << " ";
            if((printCount + 1) % 16 == 0) //If the final byte in printline.
            {   std::cout << "*";
                for(uint32_t i = printCount - 15; i < printCount + 1; ++i) //Append character printmap.
                {
                    uint8_t ch = get8(i);
                    ch = isprint(ch) ? ch : '.'; //Store "." if not printable char.
                    std::cout << ch;
                }
                std::cout << "*" << std::endl;
            }
            else if(j != 0 && (j + 1) % 8 == 0) //Space out each 8 bytes.
            {
                std::cout << " ";
            }
            ++printCount; //Increment bytes printed.
        }
    }
}

/**
 * @brief Load file into simulated memory.
 * 
 * Attempt to load files into memory.
 * 
 * @param fname Name of file within accessible directory.
 * @return true if file loading was successful and values could be written to memory.
 * @return false if file could not be opened or filesize exceeds program.
 */
bool memory::load_file(const std::string &fname)
{
    std::ifstream infile(fname, std::ios::in|std::ios::binary); //Create file object and open file in binary mode.

    if(infile.is_open())
    {
        uint8_t i;
        infile >> std::noskipws; //Prep stream extraction.
        for(uint32_t addr = 0; infile >> i; ++addr)
        {
            if(!check_illegal(addr)) //Check validity of index address before writing vlaues.
            {
                mem[addr] = i;
            }
            else
            {
                std::cerr << "Program too big." << std::endl;
                infile.close(); //Close the file.
                return false;
            }
        }
        infile.close(); //Close the file.
        return true;
    }

    std::cerr << "Can't open file '"  << fname << "' for reading." << std::endl;
    return false;
}