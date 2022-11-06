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
#include <stdlib.h> //For srand & rand.

constexpr int rows = 1000;  /// < the number of rows in the work matrix
constexpr int cols = 100;   /// < the number of cols in the work matrix
int work[rows][cols];       /// < the matrix to be summed

std::mutex stdout_lock;         /// < for serializing access to stdout
std::mutex counter_lock;        /// < for dynamic balancing only
volatile int counter = rows;    /// < for dynamic balancing only

std::vector <int> tcount;       /// < count of rows summed for each thread
std::vector <uint64_t> sum;     /// < the calculated sum from each thread

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
    bool useDynamic = false;
    int maxThreads = std::thread::hardware_concurrency();
    int numThreads = 2;
    srand(0x1234);

	int opt;
	while ((opt = getopt(argc, argv, "dt:")) != -1) //Test input arguments.
	{
		switch(opt) //Switch on command line argument.
		{
			case 'd': {useDynamic = true} break; 

			case 't': 
            {
                int requestThreads;
                std::istringstream iss(optarg);
				iss >> requestThreads;

                if(requestThreads <= maxThreads)
                {
                    numThreads = requestThreads;
                }
                else
                {
                    numThreads = maxThreads;
                }
            } 
            break; 

		default: /* '?' */
			usage(); //Specify usage information on invalid arguments.
		}
	}

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            work.at(i,j) = rand();
        } 
    }

    std::cout << maxThreads << " concurrent threads supported." << std::endl;

	return 0;
}
