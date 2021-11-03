#include "receiver.hpp"

#include <algorithm>
#include <list>
#include <string>

Receiver::Receiver(in_addr_t ip, unsigned short port, int process_id)
    : UDPserver::UDPserver(ip, port), process_id(process_id), can_receive(true)
    {}

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

    if (!process_delivered.contains(msg)) {
        
        process_delivered.push_back(msg);
    }
}

std::list<std::string> Receiver::getMessagesDelivered() const {
    
    if (!can_receive) {
        return process_delivered.getList();
    } else {
        return std::list<std::string> ();
    }
}

void Receiver::addMessageLog(const std::string& msg){

    if (!process_log.contains(msg)) {
        
        process_log.push_back(msg);
    }
}

std::list<std::string> Receiver::getMessageLog() const {
    
    if (!can_receive) {
        return process_log.getList();
    } else {
        return std::list<std::string> ();
    }
}