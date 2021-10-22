#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

/** @class Implements a UDP server */
class UDPserver {

    public:

    /** @brief The class constructor
     * @param ip of the server
     * @param port of the server
     */
    UDPserver(in_addr_t ip, unsigned short port);

    virtual ~UDPserver();

    /** @brief Receives a message
     * @param buffer where message will be stored
     */
    virtual ssize_t receive(char* buffer);

    private:

    int socketDescriptor;

    /** @brief This servers address */
    struct sockaddr_in server_addr;

};


#endif