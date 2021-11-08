#include "rb.hpp"

ReliableBroadcast::ReliableBroadcast(Receiver* receiver)
    : BestEffortBroadcast(receiver)
    {}

void ReliableBroadcast::deliver(const std::string& msg) {

    if (!messages_delivered.contains(msg)) {
        
        BestEffortBroadcast::deliver(msg);

        this->addMessage(msg);
        //this->startBroadcast(); SEGFAULT
    }
}