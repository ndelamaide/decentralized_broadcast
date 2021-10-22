#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>

/** @brief Implements a udp client */
class UDPclient {

    public:

    /** @brief The class constructor
     * @param to_addr the adress to send messages to
     */
    UDPclient(sockaddr_in target_addr);

    virtual ~UDPclient();

    /** @brief Sends a message to target
     * @param msg message to send
     * @return ssize_t of message sent or not
     */
    ssize_t send(const std::string& msg);

    /** @brief Sends a message
     * @param msg message to send
     * @param to_addr address where to send the message
     * @return ssize_t of message sent or not
     */
    ssize_t send(const std::string& msg, sockaddr_in to_addr);

    private:

    int socketDescriptor;
    sockaddr_in target_addr;

};


#endif