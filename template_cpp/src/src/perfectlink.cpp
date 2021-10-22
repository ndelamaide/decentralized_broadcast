#include "perfectlink.hpp"

#include <string>
#include <optional>
#include <thread>

#define MAX_LENGTH 32

Perfectlink::Perfectlink(Receiver* receiver, Sender* sender)
    : receiver(receiver), sender(sender), link_active(false), pop_queue(false)
    {
        link_delivered.clear();

        deliver_thread = std::thread(&Perfectlink::deliverThreaded, this);
        send_thread = std::thread(&Perfectlink::sendThreaded, this);
    }

Perfectlink::~Perfectlink() {

}

void Perfectlink::addMessage(const std::string& msg){
    messages_to_send.push(msg);
}

void Perfectlink::setLinkActive() {
    pop_queue = true;
    link_active = true;
    if (this->sender != nullptr) {
        this->sender->setCanSend(true);
    }
}

void Perfectlink::setLinkInactive() {
    pop_queue = false;
    link_active = false;
    if (this->sender != nullptr) {
        this->sender->setCanSend(false);
    }
}

void Perfectlink::sendThreaded() {

    while(true) {
        if (link_active) {

            std::optional<std::string> message_to_send = std::nullopt;

            if (pop_queue){
                message_to_send = messages_to_send.pop();
                
                if (message_to_send){
                    std::cout << "message to send poped " << *message_to_send << std::endl;
                    pop_queue = false;
                }
            }

            if (this->sender != nullptr) {

                if (message_to_send) {
                    this->sender->setMessageToSend(*message_to_send);
                }

                this->sender->send();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
}

void Perfectlink::deliverThreaded() {

    while(true) {
        if (link_active) {

            char buffer[MAX_LENGTH];

            if (!(this->receiver->receive(buffer) < 0)) {
                
                std::string received_message(buffer);

                if (link_delivered.find(received_message) == link_delivered.end()) {

                    link_delivered.insert(received_message);
                    //pop_queue = true; // NOT THE SAME PERFECT LINK SENDING AND DELIVERING !!
                    
                    std::cout << "link delivered " << received_message << std::endl;

                    // TODO @ DELIVER TO RECEIVER TOO + add mutex
                    std::lock_guard<std::mutex> lock(receiver_mutex);
                    this->receiver->addMessageDelivered(received_message);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

