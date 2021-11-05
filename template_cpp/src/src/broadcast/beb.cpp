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

    for (auto& message: messages_to_broadcast) {
        this->addSentMessageLog(message);
    }

    messages_to_broadcast.clear();

    this->setBroadcastActive();
}

void BestEffortBroadcast::deliver(const std::string& msg) {
    
    this->addDeliveredMessageLog(msg);

    std::lock_guard<std::mutex> lock(receiver_mutex);
    this->receiver->addMessageDelivered(msg);
}