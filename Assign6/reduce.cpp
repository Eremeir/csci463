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
#include <thread>
#include <mutex>
#include <vector>
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
	std::cerr << "Usage: reduce [-d] [-t num]" << std::endl;
	std::cerr << "    -d use dynamic load balancing (default uses static load balancing)" << std::endl;
	std::cerr << "    -t specify number of threads to use (default starts two threads)" << std::endl;
	exit(1); //Terminate program.
}

/**
 * @brief Sum with static load balancing.
 *
 * Sum the cells of the work matrix with worload divided evenly by the number of cooperating threads.
 * 
 * @param tid Thread ID indicating which thread is running this function.
 * @param num_threads Total number of threads executing this function.
 */
void sum_static(int tid, int num_threads)
{
    int cellCount = 0;
    uint64_t cellSum = 0;
    stdout_lock.lock();
    std::cout << "Thread " << tid << " starting" << std::endl;
    stdout_lock.unlock();

    for(int i = tid; i < rows; i+= num_threads) //Add all cells in matrix, dividing work up flatly by threads contributing.
    {
        cellCount++;
        for(int j = 0; j < cols; j++)
        {
            cellSum += work[i][j];
        } 
    }

    tcount.push_back(cellCount);
    sum.push_back(cellSum);

    stdout_lock.lock();
    std::cout << "Thread " << tid << " ending tcount=" << cellCount << " sum=" << cellSum << std::endl;
    stdout_lock.unlock();
}

/**
 * @brief Sum with dynamic load balancing.
 * 
 * @param tid Thread ID
 */
void sum_dynamic(int tid)
{
    stdout_lock.lock();
    std::cout << "Thread " << tid << " starting" << std::endl;
    stdout_lock.unlock();








    stdout_lock.lock();
    std::cout << "Thread " << tid << " ending" << std::endl;
    stdout_lock.unlock();
}

/**
 * @brief Multithreaded Matrix Summation Subroutine
 *
 * Sum all the cells of a randomly populated matrix using static or dynamic load balancing then output results.
 * 
 * @param argc Count of arguments entered.
 * @param argv Argument variables.
 * @return int 0 to signal end of program execution.
 */
int main(int argc, char **argv)
{
    std::vector<std::thread*> threads;

    bool useDynamic = false;
    int maxThreads = std::thread::hardware_concurrency();
    int numThreads = 2;
    int total_work = 0;
    uint64_t gross_sum = 0;
    srand(0x1234);

	int opt;
	while ((opt = getopt(argc, argv, "dt:")) != -1) //Test input arguments.
	{
		switch(opt) //Switch on command line argument.
		{
			case 'd': {useDynamic = true;} break; //If d flag specified, threads are to use dynamic load balancing.

			case 't': //If t flag specified, use requested thread count if possible.
            {
                int requestThreads;
                std::istringstream iss(optarg);
				iss >> requestThreads;

                if(requestThreads <= maxThreads) //If requested number of threads are not more than is available.
                {
                    numThreads = requestThreads;
                }
                else                            //Use the maximum available if requested number is too high.
                {
                    numThreads = maxThreads;
                }
            } 
            break; 

		default: /* '?' */
			usage(); //Specify usage information on invalid arguments.
		}
	}

    std::cout << maxThreads << " concurrent threads supported." << std::endl;
    
    for(int i = 0; i < rows; i++) //Populate matrix with values from srand.
    {
        for(int j = 0; j < cols; j++)
        {
            work[i][j] = rand();
        } 
    }

    if(useDynamic) //If using dynamic load balancing.
    {   
        for(int k = 0; k < numThreads; ++k)
        {
            threads.push_back(new std::thread(sum_dynamic, k));
        }
    }
    else           //Using static load balancing.
    {
        for(int k = 0; k < numThreads; ++k)
        {
            threads.push_back(new std::thread(sum_static, k, numThreads));
        }
    }

    for(int t = 0; t < numThreads; ++t) //Join and delete threads.
    {
        threads.at(t)->join();
        delete threads.at(t);
    }

    for(auto &i : tcount) //Total up workload shares.
    {
        total_work += i;
    }

    for(auto &i : sum) //Total up sum from full matrix.
    {
        gross_sum += i;
    }

    std::cout << "main() exiting, total_work=" << total_work << " gross_sum=" << gross_sum << std::endl;

	return 0;
}
