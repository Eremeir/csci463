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
 * @brief Main Memory simulation subroutine.
 * 
 * Create and test operation of simulated memory.
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

		default:
			usage(); //Specify usage information on invalid arguments.
		}
	}

	if (optind >= argc)
		usage();	// missing filename

	memory mem(memory_limit); //Create and initialize memory by set memory limit.
	mem.dump();

	if (!mem.load_file(argv[optind])) //Test if file opened and loaded values.
		usage();

	mem.dump();	//Dump memory and test functions.

	cout << mem.get_size() << endl;
	cout << hex::to_hex32(mem.get8(0)) << endl;
	cout << hex::to_hex32(mem.get16(0)) << endl;
	cout << hex::to_hex32(mem.get32(0)) << endl;
	cout << hex::to_hex0x32(mem.get8(0)) << endl;
	cout << hex::to_hex0x32(mem.get16(0)) << endl;
	cout << hex::to_hex0x32(mem.get32(0)) << endl;
	cout << hex::to_hex8(mem.get8(0)) << endl;
	cout << hex::to_hex8(mem.get16(0)) << endl;
	cout << hex::to_hex8(mem.get32(0)) << endl;

	cout << hex::to_hex0x32(mem.get32(0x1000)) << endl;

	mem.set8(0x10, 0x12);
	mem.set16(0x14, 0x1234);
	mem.set32(0x18, 0x87654321);


	cout << hex::to_hex0x32(mem.get8_sx(0x0f)) << endl;
	cout << hex::to_hex0x32(mem.get8_sx(0x7f)) << endl;
	cout << hex::to_hex0x32(mem.get8_sx(0x80)) << endl;
	cout << hex::to_hex0x32(mem.get8_sx(0xe3)) << endl;

	cout << hex::to_hex0x32(mem.get16_sx(0xe0)) << endl;
	cout << hex::to_hex0x32(mem.get32_sx(0xe0)) << endl;

	mem.dump();

	return 0;
}
