/* 
 * File:   ServerTest.cpp
 * Author: Pablo Muñoz
 */

#include <cstdlib>
#include <iostream>
#include <csignal>
#include "ServerSelect.h"
using namespace std;

ServerSelect server;

/** Capture SIGINT to close client */
void ctrlc(int sig)
{
    std::cout << "Closing connection" << std::endl;  
    close(server.getSocket());
    exit(0);
}

/** Print how to use and exit. */
void usage()
{
    std::cout << "Usage: servertest PORT" << std::endl;
    exit(-1);
}

/** Program to test the TCP/IP connector for the server logger */
int main(int argc, char** argv) 
{
    int port;
    if(argc != 2)
        usage();
    else
    {
        port = atoi(argv[1]);
        if(port < 1 || port > 65535)
        {
            std::cout << "Invalid port" << std::endl;
            usage();
        }
    }
    // Capture sigint to close the server
    signal(SIGINT, &ctrlc);

    std::cout << "Opening localhost server at "<<port<<" ... ";
    if(!server.open(port))
    {
        std::cout << "cannot open server" << std::endl;
        return -1;
    }
    std::cout << "server open" << std::endl;

    char message[M_LENGTH];
    int read = 1;
    while(read > -1)
    {
        read = server.receive(message);
        if(read > 1)
           std::cout << message << endl;
    }
    return 0;
}
