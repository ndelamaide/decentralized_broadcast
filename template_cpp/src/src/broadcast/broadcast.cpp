#include "broadcast.hpp"

#include "perfectlink.hpp"

Broadcast::Broadcast(Receiver* receiver, std::vector<Perfectlink*> links)
    :receiver(receiver), links(links)
    {}


Broadcast::~Broadcast() {
    for(auto& link: links) {
        link = nullptr;
    }
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
