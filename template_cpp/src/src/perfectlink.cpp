#include "perfectlink.hpp"
#include "broadcast.hpp"

#include <string>
#include <optional>
#include <thread>
#include <algorithm>
#include <list>

#define MAX_LENGTH 32

Perfectlink::Perfectlink(Receiver* receiver, Sender* sender, Broadcast* broadcast)
    : receiver(receiver), sender(sender), broadcast(broadcast), link_active(false), add_to_sent(true)
    {
        deliver_thread = std::thread(&Perfectlink::deliverThreaded, this);
        send_thread = std::thread(&Perfectlink::sendThreaded, this);
        ack_thread = std::thread(&Perfectlink::ackThreaded, this);

        if (this->sender != nullptr) {
            target_id = this->sender->getTargetId();
        }
        this_process_id = this->receiver->getProcessId();
    }

Perfectlink::~Perfectlink() {

}

void Perfectlink::addMessage(const std::string& msg){

    char packet[MAX_LENGTH] = {0};
    int ack = 0;
    sprintf(packet, "%-1d%03d%03d%-s", ack, this_process_id, target_id, msg.c_str());

    messages_to_send.push_back(packet);
}

void Perfectlink::addMessages(const std::list<std::string>& msgs){
    for(auto& msg: msgs){
        this->addMessage(msg);
    }
}

void Perfectlink::setLinkActive() {
    link_active = true;
    if (this->sender != nullptr) {
        this->sender->setCanSend(true);
    }
}

void Perfectlink::setLinkInactive() {
    link_active = false;
    if (this->sender != nullptr) {
        this->sender->setCanSend(false);
    }
}

void Perfectlink::sendThreaded() {

    while(true) {
        if (link_active) {
            if (this->sender != nullptr) {

                std::lock_guard<std::mutex> lock(messages_to_send_mutex);

                if (!messages_to_send.empty()) {

                    for(auto& message_to_send: messages_to_send) {
                        this->sender->sendMessage(message_to_send);

                        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
}

void Perfectlink::deliverThreaded() {

    while(true) {
        if (link_active) {

            char buffer[MAX_LENGTH];

            if (!(this->receiver->receive(buffer) < 0)) {
                
                std::string message_received(buffer);

                std::string ack = message_received.substr(0, 1);
                int msg_process_id = stoi(message_received.substr(1, 3));
                int msg_target_id = stoi(message_received.substr(4, 3));
                std::string payload = message_received.substr(7, message_received.size());
                std::string message_to_deliver = message_received.substr(1,  message_received.size());

                // Only deliver messages from the target of this link
                if ((target_id == msg_process_id) & (ack == "0")) {

                    if (!link_delivered.contains(message_to_deliver)) {
                        link_delivered.push_back(message_to_deliver);
                    }

                    broadcast_mutex.lock();
                    broadcast->deliver(message_to_deliver);
                    broadcast_mutex.unlock();

                    //send an ack as long as we receive the same message
                    message_received[0] = '1';

                    std::lock_guard<std::mutex> lock(acks_to_send_mutex);
                    acks_to_send.push_back(message_received);
                    
                // Received an ack for a message we sent to target process
                } else if ((this_process_id == msg_process_id) & (target_id == msg_target_id) & (ack == "1")) {
                    
                    std::lock_guard<std::mutex> lock(messages_to_send_mutex);

                    message_received[0] = '0';
                    messages_to_send.remove(message_received);
                }
            }

            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void Perfectlink::ackThreaded() {

    while(true) {
        if (link_active) {
            if (this->sender != nullptr) {
            
                std::lock_guard<std::mutex> lock(acks_to_send_mutex);

                acks_to_send.unique();
                
                if (!acks_to_send.empty()) {

                    for(auto& ack_to_send: acks_to_send) {
                        this->sender->sendMessage(ack_to_send);

                        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                    acks_to_send.clear();
                }
            }
            //std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
}