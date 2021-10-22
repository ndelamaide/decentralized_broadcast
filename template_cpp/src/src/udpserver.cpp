#include "udpserver.hpp"

#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include <cstring>

#define MAX_LENGTH 32

UDPserver::UDPserver(in_addr_t ip, unsigned short port){
    
    // Create socket
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketDescriptor < 0) {
        std::cerr << "Cannot create socket" << std::endl;
        exit(0);
    } 

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = ip;
    server_addr.sin_port = port;
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    if(bind(socketDescriptor, reinterpret_cast<struct sockaddr* const>(&server_addr), sizeof(server_addr)) < 0 ){
        std::cout << " Could bind to socket" << std::endl;
    }
}

UDPserver::~UDPserver(){
    close(socketDescriptor);
}

ssize_t UDPserver::receive(char* buffer){

    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    ssize_t res_rec = recvfrom(socketDescriptor, buffer, MAX_LENGTH, 0, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);

    if (res_rec < 0){
        std::cerr << "Error receiving message" << std::endl;
    }

    return res_rec;
}