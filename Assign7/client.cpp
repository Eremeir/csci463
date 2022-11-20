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
#include <string>
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
 * @brief Print Server Response
 *
 * Read the server's response from the socket buffer and write to stdout.
 * 
 * @param fd File descriptor to read from to.
 * @return int 
 */
static int print_response(int fd)
{
    char responseBuffer[2048];
    int returnVal = 1; //prime the while loop.
    while(returnVal > 0) //While read returns that the length read was above 0.
    {
        if((returnVal = read(fd, responseBuffer, sizeof(responseBuffer))) < 0)
        {
            perror("reading stream message");
            return -1; //Let caller know that read operation failed.
        }
        
        if (safe_write(fileno(stdout), responseBuffer, returnVal) < 0) //Write from the response buffer to stdout.
        {
            perror("writing on stream socket");
        }
    }
    return 0; //all went OK
}

/**
 * @brief Print usage statement.
 * 
 * Print argument usage statements and terminate program.
 * 
 */
static void usage()
{
	std::cerr << "Usage: client [-s server-ip] server-port" << std::endl;
	std::cerr << "    -s Specify the server's IPv4 number in dotted-quad format. (By default, use 127.0.0.1)." << std::endl;
    std::cerr << "    server-port The server port number to which the client must connect. " << std::endl;
	exit(1); //Terminate program.
}

/**
 * @brief Client access program.
 *
 * Create a socket and initiate a connection with the socket supplied in the command line. 
 * Then, send input from stdin and recieve a response from the server, writing that to stdout.
 *
 * @param argc Count of arguments entered.
 * @param argv Argument variables.
 * @return int 0 to signal end of program execution.
 */
int main(int argc, char *argv[])
{
    std::string IPNum = "127.0.0.1"; //Default IP to connect to.
    int opt; //For getopt
    while ((opt = getopt(argc, argv, "s:")) != -1) //Test input arguments.
    {
        switch(opt) //Switch on command line argument.
        {
            case 's': //If the s flag is specified, connect to a specific IP.
			{
				std::istringstream iss(optarg);
                IPNum = iss.str();
			}
            break;

        default: /* '?' */
            usage(); //Specify usage information on invalid arguments.
        }
    }

    signal(SIGPIPE, SIG_IGN); //Ignore the sigpipe signal.

    if(optind >= argc) //If port number was not supplied.
    {
        std::cerr << "ERROR: missing port number argument" << std::endl;
        usage();
    }

    int sock;
    struct sockaddr_in server;
    
    sock = socket(AF_INET, SOCK_STREAM, 0); //Create socket.
    if (sock < 0)
    {
        perror("opening stream socket");
        exit(1);
    }
    server.sin_family = AF_INET; //Using IPV4

    if(inet_pton(AF_INET, IPNum.c_str(), &server.sin_addr) <= 0) //Convert IPV4 and IPV6 text addresses to binary form.
    {
        std::cerr << IPNum <<": invalid address/format" << std::endl;
        exit(2);
    }

    server.sin_port = htons(atoi(argv[optind]));

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0) //Connect stream socket to server.
    {
        perror("connecting stream socket");
        exit(1);
    }

    char inputBuffer[2048];
    ssize_t returnVal;
    do
    {
        if((returnVal = read(fileno(stdin), inputBuffer, sizeof(inputBuffer))) < 0) //Read from stdin to the input buffer.
        {
            perror("reading stream message");
        }

        if (safe_write(sock, inputBuffer, returnVal) < 0) //Write from the input buffer through the socket connection.
        {
            perror("writing on stream socket");
        }

    } while(returnVal != 0); //Read from the input buffer until read returns that the length read was 0.

    shutdown(sock, SHUT_WR); //Shut the write pipe.
    print_response(sock); //Read from socket and write response to stdout.

    close(sock); //Close the socket.
    return 0;
}
