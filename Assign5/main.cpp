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
#include "cpu_single_hart.h"

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
	cerr << "Usage: rv32i [-d] [-i] [-r] [-z] [-l exec-limit] [-m hex-mem-size] infile" << endl;
	cerr << "    -d show disassembly before program execution" << endl;
	cerr << "    -i show instruction printing during execution" << endl;
	cerr << "    -l maximum number of instructions to exec" << endl;
	cerr << "    -m specify memory size (default = 0x100)" << endl;
	cerr << "    -r show register printing during execution" << endl;
	cerr << "    -z show a dump of the regs & memory after simulation" << endl;
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
	uint64_t exec_limit = 0;
	bool preDisassembly = false;
	bool showInstructions = false;
	bool showRegisters = false;
	bool postDump = false;

	int opt;
	while ((opt = getopt(argc, argv, "dil:m:rz")) != -1) //Test input arguments.
	{
		switch(opt) //Switch on command line argument.
		{
			case 'd': { preDisassembly = true; } break; //If -d flag specified, show a disassembly of the entire memory before program simulation begins.

			case 'i': { showInstructions = true; } break; //If -i flag specified, show instruction printing during execution.

			case 'l': //If -l flag specified, update maximum limit of instructions to execute. Zero means there is no limit.
			{
				std::istringstream iss(optarg);
				iss >> exec_limit;
			}
			break;

			case 'm': //If -m flag specified, update memory limit with new value.
			{
				std::istringstream iss(optarg);
				iss >> std::hex >> memory_limit;
			}
			break;

			case 'r': { showRegisters = true; } break; //If -r flag specified, show a dump of the hart (GP-registers and pc) status before each instruction is simulated.

			case 'z': { postDump = true; } break; //If -z flag specified, show a dump of the hart status and memory after the simulation has halted.

		default: /* '?' */
			usage(); //Specify usage information on invalid arguments.
		}
	}

	if (optind >= argc)
		usage();	// missing filename

	memory mem(memory_limit); //Create and initialize memory by set memory limit.

	cpu_single_hart cpu(mem); ////Create a simulated CPU with a single hardware thread, passing in simulated memory.
	cpu.reset();
	cpu.set_show_instructions(showInstructions);
	cpu.set_show_registers(showRegisters);

	if (!mem.load_file(argv[optind])) //Test if file opened and loaded values.
		usage();

	if(preDisassembly) //Disassemble if flag specified.
	{
		disassemble(mem);
	}

	cpu.run(exec_limit);

	if(postDump) //End with dumps if flag specified.
	{
		cpu.dump();
		mem.dump();
	}

	return 0;
}
