#include "perfectlink.hpp"

#include <string>
#include <optional>
#include <thread>
#include <algorithm>
#include <list>

#define MAX_LENGTH 32

Perfectlink::Perfectlink(Receiver* receiver, Sender* sender)
    : receiver(receiver), sender(sender), link_active(false), pop_queue(false)
    {
        deliver_thread = std::thread(&Perfectlink::deliverThreaded, this);
        send_thread = std::thread(&Perfectlink::sendThreaded, this);
    }

Perfectlink::~Perfectlink() {

}

void Perfectlink::addMessage(const std::string& msg){
    messages_to_send.push(msg);
}

void Perfectlink::addMessages(const std::list<std::string>& msgs){
    for(auto& msg: msgs){
        this->addMessage(msg);
    }
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

            // Always send ack first
            message_to_send = ack_to_send.pop();

            if (this->sender != nullptr) {

                if (message_to_send) {
                    
                    std::cout << "sending ack " << *message_to_send << std::endl;
                    this->sender->setMessageToSend(*message_to_send);
                    this->sender->send();

                } else if (pop_queue) {

                    message_to_send = messages_to_send.pop();
                    
                    if (message_to_send){

                        std::cout << "sending msg " << *message_to_send << std::endl;

                        link_sent.push_back(message_to_send->substr(1, message_to_send->size()));
                        this->addSentMessageLog(*message_to_send);

                        this->sender->setMessageToSend(*message_to_send);
                        this->sender->send();

                        std::lock_guard<std::mutex> lock(pop_queue_mutex);
                        pop_queue = false;
                    }

                } else {
                    this->sender->send();
                }
            }

            //std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
                int message_id = stoi(message_received.substr(1, 3));
                std::string payload = message_received.substr(4, message_received.size());
                std::string message_to_deliver = message_received.substr(1,  message_received.size());

                // Only deliver messages from the target of this link
                if ((this->sender->getTargetId() == message_id) & (ack == "0")) {

                    if (!link_delivered.contains(message_to_deliver)) {

                        link_delivered.push_back(message_to_deliver);

                        this->addDeliveredMessageLog(message_to_deliver);

                        std::lock_guard<std::mutex> lock(receiver_mutex);
                        this->receiver->addMessageDelivered(message_to_deliver);
                    }

                    std::cout << "delivering " << message_to_deliver << std::endl;

                    //send an ack as long as we receive the same message
                    message_received[0] = '1';

                    ack_to_send.push(message_received);
                
                // Received an ack from target process
                } else if ((this->receiver->getProcessId() == message_id) & (ack == "1")) {
                    
                    std::cout << "delivering  ack " << message_to_deliver << std::endl;
                    // Received an ack for last message in queue -> can stop sending
                    if (messages_to_send.size() == 0) {

                        this->sender->setMessageToSend("");

                    } else {
                        std::lock_guard<std::mutex> lock(pop_queue_mutex);
                        pop_queue = true;
                    }
                }
            }

            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

std::list<std::string> Perfectlink::getMessagesSent() const {

    if (!link_active) {
        return link_sent.getList();
    } else {
        return std::list<std::string> ();
    }
}

void Perfectlink::addSentMessageLog(const std::string& msg) {

    std::string message_sent(msg);
    message_sent[0] = 'b';

    std::lock_guard<std::mutex> lock(receiver_mutex);
    this->receiver->addMessageLog(message_sent);
}

void Perfectlink::addDeliveredMessageLog(const std::string& msg) {

    std::string message_delivered(msg);
    message_delivered[0] = 'd';

    std::lock_guard<std::mutex> lock(receiver_mutex);
    this->receiver->addMessageLog(message_delivered);
}