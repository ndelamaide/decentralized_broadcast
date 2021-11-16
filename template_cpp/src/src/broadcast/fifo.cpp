#include "fifo.hpp"

#include <iostream>
#include <string>
#include <algorithm>

Fifo::Fifo(Receiver* receiver, unsigned long NUM_PROCESSES)
    : UniformReliableBroadcast(receiver, NUM_PROCESSES), lsn(0)
    {
        for (unsigned long i = 0; i < NUM_PROCESSES; ++i) {
            next.push_back(1);
        }
    }

void Fifo::startBroadcast() {

    for (auto& message: messages_to_broadcast) {
        lsn += 1;
        message += 'l' + std::to_string(lsn);
    }

    UniformReliableBroadcast::startBroadcast();
}

void Fifo::deliverFifo(const std::string& msg) { // called in deliverpending from urb layer
    // msg should be process_id-message-lsn

    std::list<std::string>::iterator it;

    pending_fifo_mutex.lock();
    if (!(std::find(pending_fifo.begin(), pending_fifo.end(), msg) != pending_fifo.end())) {
        
        pending_fifo.push_back(msg);
    }

    // get pending messages from sender of msg
    std::list<std::string> pending_from_sender;

    for (auto& pending_message: pending_fifo) {
        
        std::string process_id_pending = pending_message.substr(0, 3);
        std::string process_id_msg = msg.substr(0, 3);

        if (std::stoi(process_id_pending) == std::stoi(process_id_msg)) {

            unsigned long idx = pending_message.rfind('l');
            std::string sn = pending_message.substr(idx + 1, msg.size());

            std::string temp = sn + 'm' + pending_message; // put sn first so we can order according to sn

            pending_from_sender.push_back(temp);

        }
    }
    pending_fifo_mutex.unlock();

    pending_from_sender.sort(); //sort by sn

    for (auto& message_pending: pending_from_sender) {

        unsigned long idx = message_pending.find('m');

        unsigned long sn = std::stoul(message_pending.substr(0, message_pending.size() - idx));
        std::string message = message_pending.substr(idx + 1, message_pending.size());

        unsigned long s = std::stoul(message.substr(0, 3)) - 1;

        std::lock_guard<std::mutex> lock(next_mutex);

        if (sn < next[s]) {
           continue;

        } else if (sn == next[s]) {

            next[s] += 1;

            std::lock_guard<std::mutex> lock(pending_fifo_mutex);
            pending_fifo.remove(message);

            unsigned long idx = message.rfind('l');

            std::string message_to_deliver = this->toMessageFormat(message.substr(0, idx));
            
            this->addDeliveredMessageLog(message_to_deliver);

        } else if (sn > next[s]) {
            break;
        }
    }
}

void Fifo::deliverPending() {

    while(true) {
        if (active) {

            std::lock_guard<std::mutex> lock(pending_mutex);
            for (auto& msg_pending: pending) {

                if (canDeliver(msg_pending) & (!messages_delivered.contains(msg_pending))) {
            
                    messages_delivered.push_back(msg_pending);

                    this->deliverFifo(msg_pending);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
}

void Fifo::addSentMessageLog(const std::string& msg) {

    unsigned long idx = msg.find('l'); // don't add sn number in log

    Broadcast::addSentMessageLog(msg.substr(0, idx));
}