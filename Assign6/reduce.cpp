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

/**
 * @brief Print usage statement.
 * 
 * Print argument usage statements and terminate program.
 * 
 */
static void usage()
{
	cerr << "Usage: reduce [-d] [-t num]" << endl;
	cerr << "    -d use dynamic load balancing (default uses static load balancing)" << endl;
	cerr << "    -t specify number of threads to use (default starts two threads)" << endl
	exit(1); //Terminate program.
}

/**
 * @brief Sum with static load balancing.
 * 
 * @param tid Thread ID
 * @param num_threads 
 */
void sum_static(int tid, int num_threads)
{

}

/**
 * @brief Sum with dynamic load balancing.
 * 
 * @param tid Thread ID
 */
void sum_dynamic(int tid)
{

}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv)
{
    constexpr int rows = 1000;  /// < the number of rows in the work matrix
    constexpr int cols = 100;   /// < the number of cols in the work matrix
    int work[rows][cols];       /// < the matrix to be summed

    std::mutex stdout_lock;         /// < for serializing access to stdout
    std::mutex counter_lock;        /// < for dynamic balancing only
    volatile int counter = rows;    /// < for dynamic balancing only

    std::vector <int> tcount;       /// < count of rows summed for each thread
    std::vector <uint64_t> sum;     /// < the calculated sum from each thread

	int opt;
	while ((opt = getopt(argc, argv, "dt:")) != -1) //Test input arguments.
	{
		switch(opt) //Switch on command line argument.
		{
			case 'd': {} break; 

			case 't': {} break; 

		default: /* '?' */
			usage(); //Specify usage information on invalid arguments.
		}
	}

	return 0;
}
