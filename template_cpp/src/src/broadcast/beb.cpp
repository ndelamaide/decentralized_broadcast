#include "beb.hpp"

#include "perfectlink.hpp"

#include <string>

BestEffortBroadcast::BestEffortBroadcast(Receiver* receiver, bool log)
    : Broadcast(receiver, log)
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

void BestEffortBroadcast::broadcastMessage(const std::string& msg) {

    for(auto& link: links){
        if (link != nullptr) {
            link->addMessage(msg);
        }
    }
}

void BestEffortBroadcast::deliver(const std::string& msg) {
    
    if (!messages_delivered.contains(msg)) {

        messages_delivered.push_back(msg);
        
        if (log) {
            this->addDeliveredMessageLog(msg);  
        }

        std::lock_guard<std::mutex> lock(upper_layer_mutex);
        if (upper_layer != nullptr) {
            upper_layer->deliver(msg);
        }
    }
}