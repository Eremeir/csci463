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
#include <sstream>
#include <getopt.h>
#include "memory.h"
#include "rv32i_decode.h"

using std::cerr;
using std::cout;
using std::endl;

/**
 * @brief Print usage statement.
 * 
 * Print argument usage statements and terminate program.
 * 
 */
static void usage()
{
	cerr << "Usage: rv32i [-m hex-mem-size] infile" << endl;
	cerr << "    -m specify memory size (default = 0x100)" << endl;
	exit(1); //Terminate program.
}

/**
 * @brief Disassemble memory.
 * 
 * @param mem vector object to access and disassemble.
 */
static void disassemble(const memory &mem)
{
	for(uint32_t addr = 0; addr < mem.get_size(); addr+=4)
	{
		uint32_t insn = mem.get32(addr);
		std::cout << hex::to_hex32(addr) << ": ";
		std::cout << hex::to_hex32(insn) << "  " << rv32i_decode::decode(addr, insn) << std::endl;
	}
}

/**
 * @brief Disassemble and Decode Binary Main Subroutine.
 * 
 * Load data into simulated memory then decode and print out the instruction list.
 * 
 * @param argc Count of arguments entered.
 * @param argv Argument variables.
 * @return int 0 to signal end of program.
 */
int main(int argc, char **argv)
{
	uint32_t memory_limit = 0x100;	// default memory size is 0x100
	int opt;
	while ((opt = getopt(argc, argv, "m:")) != -1) //Test input arguments.
	{
		switch(opt)
		{
		case 'm': //If -m flag specified, update memory limit with new value.
			{
				std::istringstream iss(optarg);
				iss >> std::hex >> memory_limit;
			}
			break;

		default: /* '?' */
			usage(); //Specify usage information on invalid arguments.
		}
	}

	if (optind >= argc)
		usage();	// missing filename

	memory mem(memory_limit); //Create and initialize memory by set memory limit.

	if (!mem.load_file(argv[optind])) //Test if file opened and loaded values.
		usage();

	disassemble(mem);
	mem.dump();

	return 0;
}
