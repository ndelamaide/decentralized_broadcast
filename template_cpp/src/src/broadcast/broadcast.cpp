#include "broadcast.hpp"

Broadcast::Broadcast(Receiver* receiver)
    :receiver(receiver)
    {}

Broadcast::~Broadcast() {
    for(auto& link: links) {
        link = nullptr;
    }
}

void Broadcast::addLinks(std::vector<Perfectlink*> links_to_add) {
    links = links_to_add;
}

void Broadcast::addMessage(const std::string& msg){
    messages_to_broadcast.push_back(msg);
}

void Broadcast::setBroadcastActive(){
    active = true;
    for(auto& link: links) {
        if (link != nullptr) {
            link->setLinkActive();
        }        
    }
}

void Broadcast::setBroadcastInactive(){
    active = false;
    for(auto& link: links) {
        if (link != nullptr) {
            link->setLinkInactive();
        }        
    }
}

void Broadcast::addSentMessageLog(const std::string& msg) {

    std::string message_sent = 'b' + msg;

    std::lock_guard<std::mutex> lock(receiver_mutex);
    this->receiver->addMessageLog(message_sent);
}

void Broadcast::addDeliveredMessageLog(const std::string& msg) {

    std::string message_delivered(msg);
    message_delivered[0] = 'd';

    std::lock_guard<std::mutex> lock(receiver_mutex);
    this->receiver->addMessageLog(message_delivered);
}
