#include "beb.hpp"

#include "perfectlink.hpp"

BestEffortBroadcast::BestEffortBroadcast(Receiver* receiver, std::vector<Perfectlink*> links)
    :receiver(receiver), links(links)
    {}


BestEffortBroadcast::~BestEffortBroadcast() {
    for(auto& link: links) {
        link = nullptr;
    }
}

void BestEffortBroadcast::addMessage(const std::string& msg){
    messages_to_broadcast.push_back(msg);
}

void BestEffortBroadcast::setBroadcastActive(){
    active = true;
    for(auto& link: links) {
        link->setLinkActive();
    }
}

void BestEffortBroadcast::setBroadcastInactive(){
    active = false;
    for(auto& link: links) {
        link->setLinkInactive();
    }
}

void BestEffortBroadcast::startBroadcast() {

    for(auto& link: links){
        link->addMessages(messages_to_broadcast);
    }
    messages_to_broadcast.clear();

    this->setBroadcastActive();
}


