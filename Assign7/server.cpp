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

/*
 * This program creates a socket and then begins an infinite loop. Each time
 * through the loop it accepts a connection and prints out messages from it.
 * When the connection breaks, or a termination message comes through, the
 * program accepts a new connection.
 */



static ssize_t safe_write(int fd, const char *buf, size_t len)
{
    while(len > 0)
    {
        ssize_t wlen = write(fd, buf, len);
        if(wlen == -1) //write returned an unrecoverable error, errno is set.
        {
            return -1;
        }
        //std::cout << "sent " << wlen << " bytes" << std::endl;
        len -= wlen; //reduce number of remaining bytes to send
        buf += wlen; //advance the buffer pointer past the written data
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

	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("opening stream socket");
		exit(1);
	}

	if(bind(sock, (sockaddr*)&server, sizeof(server)))
	{
		perror("binding stream socket");
		exit(1);
	}
	/* Find out assigned port number and print it out */
	length = sizeof(server);
	if(getsockname(sock, (sockaddr*)&server, &length))
	{
		perror("getting socket name");
		exit(1);
	}
	std::cout << "Socket has port #" << ntohs(server.sin_port) << std::endl;

	/* Start accepting connections */
	listen(sock, 5);
	do 
	{
		struct sockaddr_in client;
		socklen_t client_len = sizeof(client);
		msgsock = accept(sock, (struct sockaddr*)&client, &client_len);
		if (msgsock == -1)
		{
			perror("accept");
		}
			
		else
		{
			inet_ntop(client.sin_family, &client.sin_addr, buffer, sizeof(buffer));
			std::cout << "Accepted connection from '" << buffer << "', port " << ntohs(client.sin_port) << std::endl;

			do
			{
				if ((returnVal = read(msgsock, buffer, sizeof(buffer))) < 0)
				{
					perror("reading stream message");
				}
				
				if (returnVal == 0)
				{
					std::cout << "Ending connection" << std::endl;
				}
				
				else
				{
					std::cout << "---- read " << returnVal << " bytes ---->>>" << buffer << "<<<" << std::endl;
				}
				
			} while(returnVal != 0);

			returnVal = safe_write(msgsock, "thank you!!!", 12);
			if(returnVal < 0)
			{
				perror("write failed");
			}
			std::cout << "---- write " << returnVal << " bytes" << std::endl;
			returnVal = safe_write(msgsock, "very much", 9);
			if(returnVal < 0)
			{
				perror("write failed");
			}
			std::cout << "---- write " << returnVal << " bytes" << std::endl;

			close(msgsock);
		}
	} while (true);
	/*
	* Since this program has an infinite loop, the socket "sock" is
	* never explicitly closed.  However, all sockets will be closed
	* automatically when a process is killed or terminates normally.
	*/
}
