#include "fifo.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <map>

Fifo::Fifo(Receiver* receiver, UniformReliableBroadcast* urb, unsigned long NUM_PROCESSES, bool log)
    : Broadcast(receiver, log), lsn(0)
    {
        this->urb = urb;
        this->urb->setUpperLayer(this);

        for (unsigned long i = 0; i < NUM_PROCESSES; ++i) {
            next.push_back(1);
        }
    }

void Fifo::startBroadcast() {

    for (auto& message: messages_to_broadcast) {
        lsn += 1;

        this->addSentMessageLog(message);

        message += 'l' + std::to_string(lsn);
        this->urb->broadcastMessage(message, bool (true));
    }

    messages_to_broadcast.clear();

    this->setBroadcastActive();
    this->urb->setBroadcastActive();
}

void Fifo::broadcastMessage(const std::string& msg) {

    lsn += 1;

    std::string message(msg);
    message += 'l' + std::to_string(lsn);
    
    this->urb->broadcastMessage(msg);
}

void Fifo::deliver(const std::string& msg) { // called in deliverpending from urb layer
    // msg should be process_id-message-lsn

    std::list<std::string>::iterator it;

    pending_mutex.lock();
    if (!(std::find(pending.begin(), pending.end(), msg) != pending.end())) {
        
        pending.push_back(msg);
    }

    // get pending messages from sender of msg
    std::map<unsigned long, std::string> pending_from_sender;

    unsigned long process_id_msg = std::stoul(msg.substr(0, 3));

    for (auto& pending_message: pending) {

        std::cout << "pending fifo " << pending_message << std::endl;
        
        unsigned long process_id_pending = std::stoul(pending_message.substr(0, 3));

        if (process_id_pending == process_id_msg) {

            unsigned long idx = pending_message.rfind('l');
            unsigned long sn = stoul(pending_message.substr(idx + 1, msg.size()));

            pending_from_sender[sn] = pending_message;
        }
    }
    pending_mutex.unlock();

    // Find messages from sender such that sn' < next[s]

    std::map<unsigned long, std::string>::iterator next_s = pending_from_sender.find(next[process_id_msg - 1]);

    while (next_s != pending_from_sender.end()) {
        next[process_id_msg - 1] += 1;

        std::string message = next_s->second;

        std::lock_guard<std::mutex> lock(pending_mutex);
        pending.remove(message);

        unsigned long idx = message.rfind('l');

        if (log) {
            
            std::string message_to_deliver = this->toMessageFormat(message.substr(0, idx));
        
            this->addDeliveredMessageLog(message_to_deliver);
        }
        
        next_s = pending_from_sender.find(next[process_id_msg - 1]);
    }
}

std::string Fifo::toMessageFormat(const std::string msg) {

    std::string message_sender = msg.substr(0, 3);
    std::string message_payload = msg.substr(3, msg.size());

    std::string ack = "0";
    std::string target = "000";

    std::string formated = ack + message_sender + target + message_payload;

    return formated;
}