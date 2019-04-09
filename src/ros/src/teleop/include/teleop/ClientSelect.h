/* 
 * File:   ClientSelect.h
 * Author: Pablo Muñoz
 * Date: June 7, 2014
 *
 * A non-blocking socket client
 */

#ifndef CLIENTSELECT_H
#define	CLIENTSELECT_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "Socket.h"


class ClientSelect {
public:
    /** Constructor. */
    ClientSelect();

    /** Closes the socket. */
    ~ClientSelect();

    /** Open a bidirectional socket with the specified server.
     @param ip IP direction of the server
     @param port port number
     @return true if the connection is stablished, false otherwise.
     */
    bool open(char* ip, int port);

    /** Get data from the server.
     @param data buffer for the data received
     @return number of bytes readed, -1 if any error.
     */
    int receive(char* data);

    /** Send data to the server.
     @param data buffer with the data to send
     @return number of bytes send, -1 if any error.
     */
    int send(char* data);

    /** @return true if the client is connected, false otherwise. */
    bool isConnected();

    /** @return Descriptor of the socket. */
    int getSocket();

private:
    /** Socket descriptor of the client. */
    int socketClient;
};

#endif	/* CLIENTSELECT_H */

