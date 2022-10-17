#ifndef H_MEM
#define H_MEM

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
#include <vector>
#include "hex.h"

/**
 * @brief Simulated Memory Class
 * 
 * Facilitates the creation and operation of simulated memory, allowing reading and writing of simulated byte data.
 * 
 */
class memory : public hex
{
public:
    memory(uint32_t size); //Constructor
    ~memory();             //Destructor

    bool check_illegal(uint32_t addr) const;  //Check index validity.
    uint32_t get_size() const;                //Get memory size.
    uint8_t get8(uint32_t addr) const;        //Get 8bits of memory.
    uint16_t get16(uint32_t addr) const;      //Get 16bits of memory.
    uint32_t get32(uint32_t addr) const;      //Get 32bits of memory.
    
    int32_t get8_sx(uint32_t addr) const;     //Get 32bits of 8 bit sign extended memory. 
    int32_t get16_sx(uint32_t addr) const;    //Get 32bits of 16 bit sign extended memory. 
    int32_t get32_sx(uint32_t addr) const;    //Get 32bits of signed memory.

    void set8(uint32_t addr, uint8_t val);    //Set 8bits of memory.
    void set16(uint32_t addr, uint16_t val);  //Set 16bits of memory.
    void set32(uint32_t addr, uint32_t val);  //Set 32bits of memory.

    void dump() const;  //Print memory dump.

    bool load_file(const std::string &fname);  //Load file into simulated memory.

private:
    std::vector<uint8_t> mem; //Vector to simulate memory.
};

#endif