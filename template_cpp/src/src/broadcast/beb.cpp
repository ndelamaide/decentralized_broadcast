#include "beb.hpp"

#include "perfectlink.hpp"

BestEffortBroadcast::BestEffortBroadcast(Receiver* receiver, std::vector<Perfectlink*> links)
    : Broadcast(receiver, links)
    {}

void BestEffortBroadcast::startBroadcast() {

    for(auto& link: links){
        if (link != nullptr) {
            link->addMessages(messages_to_broadcast);
        }
    }
    messages_to_broadcast.clear();

    this->setBroadcastActive();
}


