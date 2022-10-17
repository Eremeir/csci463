#ifndef H_REGISTER
#define H_REGISTER

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
#include "hex.h"

/**
 * @brief Simulated Register Class
 * 
 *  Facilitates the creation and operation of 32 simulated registers.
 *
 */
class registerfile : public hex
{
public:
    registerfile();  //Constructor
    ~registerfile(); //Destructor

    void reset();                        //Reset and initialize registers.
    void set(uint32_t reg, int32_t val); //Set register value.
    int32_t get(uint32_t reg);           //Return register value.
    void dump(const std::string &hdr);   //Dump register contents.
    
private:
    std::vector<int32_t> regs; //Vector to simulate registers.
};

#endif