#include "urb.hpp"

#include <string>
#include <cstring>
#include <set>
#include <thread>
#include <algorithm>

UniformReliableBroadcast::UniformReliableBroadcast(Receiver* receiver, BestEffortBroadcast* beb, unsigned long NUM_PROCESSES, bool log)
    : Broadcast(receiver, log), NUM_PROCESSES(NUM_PROCESSES)
    {
        this->beb = beb;
        this->beb->setUpperLayer(this);
        deliver_pending_thread = std::thread(&UniformReliableBroadcast::deliverPending, this);
    }


void UniformReliableBroadcast::setBroadcastActive() {
    
    this->active = true;
    this->beb->setBroadcastActive();
}

void UniformReliableBroadcast::startBroadcast() {

    for (auto& message: messages_to_broadcast) {

        receiver_mutex.lock();
        int process_id = this->receiver->getProcessId();
        receiver_mutex.unlock();

        char str_process_id[4];
        sprintf(str_process_id, "%03d", process_id);
        
        std::string message_pending = str_process_id + message;
        pending.push_back(message_pending);

        this->beb->broadcastMessage(message);
        this->addSentMessageLog(message);
    }

    messages_to_broadcast.clear();

    this->setBroadcastActive();
    this->beb->setBroadcastActive();
}

void UniformReliableBroadcast::broadcastMessage(const std::string& msg) {

    pending.push_back(msg);
    this->beb->broadcastMessage(msg);
}

void UniformReliableBroadcast::deliver(const std::string& msg) {

    std::string msg_payload = msg.substr(7, msg.size());
    std::string msg_sender = msg.substr(1, 3);

    std::string sender_message; // sender_id, message pair
    
    if (msg_payload[0] == 'r') { // if it's a relayed msg

        std::string original_process = msg_payload.substr(1, 3);
        std::string original_payload = msg_payload.substr(4, msg_payload.size());        

        sender_message = original_process + original_payload;

        ack.insert(std::make_pair(sender_message, msg_sender));

    } else {

        sender_message = msg_sender + msg_payload;

        ack.insert(std::make_pair(sender_message, msg_sender));
    }

    std::lock_guard<std::mutex> lock(pending_mutex);

    std::list<std::string>::iterator it;

    if (!(std::find(pending.begin(), pending.end(), sender_message) != pending.end())) {
        
        pending.push_back(sender_message);

        if (msg[7] == 'r') {

            this->beb->broadcastMessage(msg.substr(7, msg.size()));

        } else { // we are the first to relay this msg

            std::string message_to_relay;

            std::string original_sender = msg.substr(1, 3);
            std::string payload = msg.substr(7, msg.size());

            message_to_relay = 'r' + original_sender + payload;

            this->beb->broadcastMessage(message_to_relay);
        }
    }
}

bool UniformReliableBroadcast::canDeliver(const std::string& msg) const {
    return (ack.count(msg) > NUM_PROCESSES / 2);
}

void UniformReliableBroadcast::deliverPending() {

    while(true) {
        if (active) {

            std::lock_guard<std::mutex> lock(pending_mutex);
            for (auto& msg_pending: pending) {

                if (canDeliver(msg_pending) & (!messages_delivered.contains(msg_pending))) {
            
                    messages_delivered.push_back(msg_pending);

                    if (log) {
                        std::string message_to_deliver = this->toMessageFormat(msg_pending);

                        this->addDeliveredMessageLog(message_to_deliver);
                    }

                    std::lock_guard<std::mutex> lock(upper_layer_mutex);
                    if (upper_layer != nullptr) {
                        upper_layer->deliver(msg_pending);
                    }
                }
            }
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
}

std::string UniformReliableBroadcast::toMessageFormat(const std::string msg) {

    std::string message_sender = msg.substr(0, 3);
    std::string message_payload = msg.substr(3, msg.size());

    std::string ack = "0";
    std::string target = "000";

    std::string formated = ack + message_sender + target + message_payload;

    return formated;
}