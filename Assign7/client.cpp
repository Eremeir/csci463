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

constexpr const char data[] = "THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' ANDANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THEIMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSEARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLEFOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIALDAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODSOR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICTLIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAYOUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OFSUCH DAMAGE. THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' ANDANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THEIMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSEARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLEFOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIALDAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODSOR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICTLIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAYOUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OFSUCH DAMAGE.";

static ssize_t safe_write(int fd, const char *buffer, size_t len)
{
    while(len > 0)
    {
        ssize_t wlen = write(fd, buffer, len);
        if(wlen == -1) //write returned an unrecoverable error, errno is set.
        {
            return -1;
        }
        //std::cout << "sent " << wlen << " bytes" << std::endl;
        len -= wlen; //reduce number of remaining bytes to send
        buffer += wlen; //advance the buffer pointer past the written data
    }
    return len; //if we get here when we sent the full requested length!
}

static int print_response(int fd)
{
    char buffer[2048];
    int returnVal = 1; //prime the while loop
    while(returnVal > 0)
    {
        if((returnVal = read(fd, buffer, sizeof(buffer)-1)) == -1)
        {
            perror("reading stream message");
            return -1; //let caller know badness happened
        }
        else if(returnVal > 0)
        {
            buffer[returnVal] = '\0';
            std::cout << "---- read " << returnVal << " bytes ---->>>" << buffer << "<<<" << std::endl;
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

/*
 * This program creates a socket and initiates a connection with the socket
 * given in the command line.  One message is sent over the connection and
 * then the socket is closed, ending the connection. The form of the command
 * line is streamwrite hostname portnumber
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

    if(optind >= argc)
    {
        std::cerr << "ERROR: missing port number argument" << std::endl;
        usage();
    }

    int sock;
    struct sockaddr_in server;
    
    /* Create socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("opening stream socket");
        exit(1);
    }
    /* Connect socket using name specified by command line. */
    server.sin_family = AF_INET; //Using IPV4

    if(inet_pton(AF_INET, IPNum.c_str(), &server.sin_addr) <= 0) //Convert IPV4 and IPV6 text addresses to binary form.
    {
        std::cerr << IPNum <<": invalid address/format" << std::endl;
        exit(2);
    }

    server.sin_port = htons(atoi(argv[optind]));

    if (connect(sock, (sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("connecting stream socket");
        exit(1);
    }
    (void)data;

    char inputBuffer[2048];
    ssize_t returnVal;
    do
    {
        if((returnVal = read(fileno(stdin), inputBuffer, sizeof(inputBuffer))) < 0) //Read from stdin to the input buffer.
        {
            perror("reading stream message");
        }

    } while(returnVal != 0);

    if (safe_write(sock, inputBuffer, sizeof(inputBuffer)) < 0) //Write from the input duffer through the socket connection.
    {
        perror("writing on stream socket");
    }

    shutdown(sock, SHUT_WR);
    print_response(sock);

    close(sock);
    return 0;
}
