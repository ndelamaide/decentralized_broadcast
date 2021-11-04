#include "beb.hpp"

#include "perfectlink.hpp"

BestEffortBroadcast::BestEffortBroadcast(Receiver* receiver)
    : Broadcast(receiver)
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

void BestEffortBroadcast::deliver(const std::string& msg) {
    // Add to broadcast delivered
    std::string test = msg;
}
