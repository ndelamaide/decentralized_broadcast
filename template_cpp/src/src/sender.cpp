#include "sender.hpp"

#include <string>

Sender::Sender(sockaddr_in target_addr)
    : UDPclient::UDPclient(target_addr), msg_to_send(""), can_send(false)
    {}

Sender::~Sender(){
    UDPclient::~UDPclient();
}

ssize_t Sender::send() {

    if (msg_to_send != "") {
        return UDPclient::send(msg_to_send.c_str());
    } else {
        return -1;
    }
}

void Sender::setMessageToSend(const std::string& msg) {
    msg_to_send = msg;
}

std::string Sender::getMessageToSend() const {
    return msg_to_send;
}

void Sender::setCanSend(bool bool_) {
    can_send = bool_;
}