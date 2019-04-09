/* 
 * File:   Socket.h
 * Author: Pablo Muñoz
 * Date: June 7, 2014
 *
 * Basic socket functionality
 */

#ifndef _SOCKET_H
#define _SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

/** Size of messages sended/received. */
#define M_LENGTH 800

/** Read data from a non-blocking socket. @see recv
 * @param fd socket descriptor
 * @param data buffer for readed message
 * @param len desired size of data readed
 * @return size of the read data. 0 if no data is availble, -1 for a closed socket.
 */
int receiveSocket(int fd, char *data, int len);

/** Read data from a non-blocking socket. It expects to read a data size of M_LENGTH. @see recv
 * @param fd socket descriptor
 * @param data buffer for readed message
 * @return size of the read data. 0 if no data is availble, -1 for a closed socket.
 */
int receiveSocket(int fd, char *data);

/** Send data using the provided socket.
 * @param fd socket descriptor
 * @param data buffer with the message to send
 * @param len desired size of data readed
 * @return size of the sended data. -1 for a closed socket or a I/O problem.
 */
int sendSocket(int fd, char *data, int len);

/** Send data using the provided socket. The data size shall be M_LENGTH. Otherwhise, if less data is provided
 * it will fill the data to M_LENGTH using '\0'. If more data is given, it will be truncated to M_LENGTH.
 * @param fd socket descriptor
 * @param data buffer with the message to send with a size of M_LENGTH
 * @return size of the sended data. -1 for a closed socket or a I/O problem.
 */
int sendSocket(int fd, char *data);

#endif
