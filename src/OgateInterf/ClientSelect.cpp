/* 
 * File:   ClientSelect.h
 * Author: Pablo Muñoz
 * Date: June 7, 2014
 *
 * Implementaton of a non-blocking socket client
 */

#include "ClientSelect.h"

ClientSelect::ClientSelect() {
    socketClient = -1;
}

ClientSelect::~ClientSelect() {
    close(socketClient);
}

bool ClientSelect::open(char* ip, int port)
{
    // Check parameters
    if(ip == NULL || port < 0 || port > 65535)
        return false;

    // Get host from ip
	struct sockaddr_in dir;
	struct hostent *Host;
	Host = gethostbyname(ip);
	if(Host == NULL)
		return -1;

	dir.sin_family = AF_INET;
	dir.sin_addr.s_addr = ((struct in_addr *)(Host->h_addr))->s_addr;
	dir.sin_port = ntohs(port);
	
    // Open socket
	socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if(socketClient == -1)
		return -1;
    // Connect to the server
	if(connect(socketClient, (struct sockaddr*)&dir, sizeof(dir)) == -1)
		return -1;
	// Set non-block option
    fcntl(socketClient, F_SETFL, O_NONBLOCK);

    if(socketClient < 0)
        return false;
    else
        return true;
}

bool ClientSelect::isConnected()
{
    return socketClient >= 0;
}

int ClientSelect::receive(char* data)
{
    return receiveSocket(socketClient, data);
}

int ClientSelect::send(char* data)
{
    return sendSocket(socketClient, data);
}

int ClientSelect::getSocket()
{
    return socketClient;
}
