/* 
 * File:   Socket.cpp
 * Author: Pablo Muñoz
 * Date: June 7, 2014
 *
 * Basic socket functionality implementation
 */

#include "Socket.h"


int receiveSocket(int fd, char* data, int len)
{
	// Check if the socket is open and the paramters are correct
	if(fd < 0 || data == NULL || len < 1)
		return -1;

    return recv(fd, data, len, O_NONBLOCK);
}

int receiveSocket(int fd, char* data)
{
    return receiveSocket(fd, data, M_LENGTH);
}

int sendSocket(int fd, char* data, int len)
{
    // Check if the socket is open and the parameters are correct   
	if(fd < 0 || data == NULL || len < 1)
		return -1;

    return write(fd, data, len);
}

int sendSocket(int fd, char* data)
{
    if(fd < 0 || data == NULL)
        return -1;
    char aux[M_LENGTH];
    // If data size < M_LENGTH it fill with '\0'
    strncpy(aux, data, M_LENGTH);
    // If data size > M_LENGTH aux is not null-terminated (data is truncated)
    if(strlen(data) > M_LENGTH)
        aux[M_LENGTH-1] = '\0';
    return write(fd, aux, M_LENGTH);
}

