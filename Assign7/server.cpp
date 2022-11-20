/*
 Copyright (c) 1986, 1993
 The Regents of the University of California.  All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. All advertising materials mentioning features or use of this software
    must display the following acknowledgement:
 This product includes software developed by the University of
 California, Berkeley and its contributors.
 4. Neither the name of the University nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.

    Modifications to make this build & run on Linux by John Winans, 2021
*/

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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <getopt.h>

/**
 * @brief Safe Write
 *
 * Properly write the whole buffer ensuring all bytes werw written.
 * 
 * @param fd File descriptor to write to.
 * @param buffer of bytes to write from.
 * @param len length of the buffer occupied by bytes.
 * @return ssize_t return code to indicate successes or failure.
 */
static ssize_t safe_write(int fd, const char *buffer, size_t len)
{
    while(len > 0) //While write returns that the length read was above 0.
    {
        ssize_t wlen = write(fd, buffer, len);
        if(wlen == -1) //write returned an unrecoverable error, errno is set.
        {
            return -1;
        }
        len -= wlen; //decrement number of remaining bytes to send by length written.
        buffer += wlen; //advance the buffer pointer by length written.
    }
    return len; //if we get here when we sent the full requested length!
}

/**
 * @brief Print usage statement.
 * 
 * Print argument usage statements and terminate program.
 * 
 */
static void usage()
{
	std::cerr << "Usage: server [-l listener-port]" << std::endl;
	std::cerr << "    -l The port number to which the server must listen. By default, the port number should be zero (wildcard/ephemeral)." << std::endl;
	exit(1); //Terminate program.
}

/**
 * @brief Server host program.
 *
 * Create a socket and start an infinite loop, accepting connections and calculating a checksum from read messages. 
 * When the connection closes or terminates, new connections may be established.
 *
 * @param argc Count of arguments entered.
 * @param argv Argument variables.
 * @return int 0 to signal end of program execution.
 */
int main(int argc, char **argv)
{
	int sock;
	socklen_t length;
	struct sockaddr_in server;
	int msgsock;
	char buffer[2048];
	int returnVal;

	/* Name socket using wildcards */
	server.sin_family = AF_INET; //Using IPV4.
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = 0; //Ephermeral port.

	int opt; //For getopt
    while ((opt = getopt(argc, argv, "l:")) != -1) //Test input arguments.
    {
        switch(opt) //Switch on command line argument.
        {
            case 'l': //If the l flag is specified, listen on a specific port number.
			{
				uint16_t portNum;
				std::istringstream iss(optarg);
				iss >> portNum;
				server.sin_port = htons(portNum); //Convert port number integer to network byte order.
			}
            break;

        default: /* '?' */
            usage(); //Specify usage information on invalid arguments.
        }
    }

	signal(SIGPIPE, SIG_IGN); //Ignore the sigpipe signal.

	sock = socket(AF_INET, SOCK_STREAM, 0); //Create socket.
	if (sock < 0)
	{
		perror("opening stream socket");
		exit(1);
	}

	if(bind(sock, (sockaddr*)&server, sizeof(server))) //Bind address to socket.
	{
		perror("binding stream socket");
		exit(1);
	}

	if(getsockname(sock, (sockaddr*)&server, &length))  //Find out assigned port number and print it out.
	{
		perror("getting socket name");
		exit(1);
	}
	std::cout << "Socket has port #" << ntohs(server.sin_port) << std::endl;

	listen(sock, 5); //Start listening for incoming connections.
	do 
	{
		struct sockaddr_in client;
		socklen_t client_len = sizeof(client);
		msgsock = accept(sock, (struct sockaddr*)&client, &client_len); //Accept connection.
		if (msgsock == -1)
		{
			perror("accept");
		}
			
		else
		{ 
			uint32_t byteCount = 0; //To count total number of bytes.
			uint16_t checksum = 0; 	//To sum from each byte,
			uint8_t bufferByte = 0; //To hold a byte temporarily for summing.

			inet_ntop(client.sin_family, &client.sin_addr, buffer, sizeof(buffer)); //Convert the network address into a character string.
			std::cout << "Accepted connection from '" << buffer << "', port " << ntohs(client.sin_port) << std::endl;

			do
			{
				if ((returnVal = read(msgsock, buffer, sizeof(buffer))) < 0) //Read from the socket to buffer.
				{
					perror("reading stream message");
				}
				
				if (returnVal == 0) //If there is nothing left to read, print that the connection is ending.
				{
					std::cout << "Ending connection" << std::endl;
				}
				
				else
				{
					for(ssize_t i = 0; i < returnVal; i++) //Iterate through the buffer and calculate checksum.
					{
						bufferByte = buffer[i];
						checksum += bufferByte;
					}

					byteCount += returnVal;
				}
				
			} while(returnVal != 0); //Read from the input buffer until read returns that the length read was 0.

			std::ostringstream oss;
			oss << "Sum: " << checksum << " Len: " << byteCount << "\n"; //Create a string object to send back to client.

			returnVal = safe_write(msgsock, oss.str().c_str(), oss.str().length()); //Write to socket from string object.
			if(returnVal < 0)
			{
				perror("write failed");
			}

			close(msgsock); //Close the socket.
		}
	} while (true);
	/*
	* Since this program has an infinite loop, the socket "sock" is
	* never explicitly closed.  However, all sockets will be closed
	* automatically when a process is killed or terminates normally.
	*/
	return 0;
}
