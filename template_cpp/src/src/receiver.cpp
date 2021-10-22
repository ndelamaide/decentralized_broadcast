#include "receiver.hpp"

#include <algorithm>

Receiver::Receiver(in_addr_t ip, unsigned short port, int process_id)
    : UDPserver::UDPserver(ip, port), process_id(process_id), can_receive(true)
    {
        process_delivered.clear();
    }

 Receiver::~Receiver(){
     UDPserver::~UDPserver();
 }

 ssize_t Receiver::receive(char* buffer) {

     if (can_receive) {
        return UDPserver::receive(buffer);
     } else {
         return -1;
     }
     
 }

int Receiver::getProcessId() const {
    return process_id;
}

void Receiver::setCanReceive(bool bool_) {
    can_receive = bool_;
}

void Receiver::addMessageDelivered(const std::string& msg){

    std::list<std::string>::iterator it;

    if (std::find(process_delivered.begin(), process_delivered.end(), msg) == process_delivered.end()) {
        
        process_delivered.push_back(msg);
    }
}

std::list<std::string> Receiver::getMessagesDelivered() const {
    return process_delivered;
}