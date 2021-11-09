#include "rb.hpp"

ReliableBroadcast::ReliableBroadcast(Receiver* receiver)
    : BestEffortBroadcast(receiver)
    {}



void ReliableBroadcast::deliver(const std::string& msg) {

    if (!messages_delivered.contains(msg)) {

        BestEffortBroadcast::deliver(msg);
        this->relay(msg);
    }
}

void ReliableBroadcast::relay(const std::string& msg) {
    // TODO : NEED TO ADD TO LOG ?

    for (auto& link: links) {
        if (link != nullptr) {
            link->addMessageRelay(msg);
        }
    }
}