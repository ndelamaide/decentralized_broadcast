#include "broadcast.hpp"

#define MAX_LENGTH 94

Broadcast::Broadcast(Receiver* receiver, bool log)
    :receiver(receiver), upper_layer(nullptr), log(log)
    {}

Broadcast::~Broadcast() {
    for(auto& link: links) {
        link = nullptr;
    }
}

void Broadcast::addLinks(std::vector<Perfectlink*> links_to_add) {
    links = links_to_add;
}

void Broadcast::addMessage(const std::string& msg){
    messages_to_broadcast.push_back(msg);
}

void Broadcast::setBroadcastActive(){
    active = true;
    for(auto& link: links) {
        if (link != nullptr) {
            link->setLinkActive();
        }        
    }
}

void Broadcast::setBroadcastInactive(){
    active = false;
    for(auto& link: links) {
        if (link != nullptr) {
            link->setLinkInactive();
        }        
    }
}

void Broadcast::setUpperLayer(Broadcast* upper_layer) {
    this->upper_layer = upper_layer;
}

void Broadcast::concatMessages() {

    concat = true;

    unsigned long group_by = 15; // Number of messages to concat

    std::list<std::string> messages_concat;

    unsigned long num_concat = messages_to_broadcast.size() / group_by;
    unsigned long remainder = messages_to_broadcast.size() % group_by;

    unsigned long group_num(0);
    unsigned long msg_num(0);

    std::string message_concatenated = std::to_string(group_by); // number of messages we concat in msg

    for (auto& message: messages_to_broadcast) {

        if(group_num < num_concat) {

            if (group_num == group_by - 1) { // don't add sep for last message to concat

                message_concatenated += message;

            } else {

                message_concatenated += message + 's'; 
            }

            ++msg_num;

             // If we concatenated group_by messages, push concat and create new concatenated msg
            if (msg_num == group_by) {
                ++group_num;
                msg_num = 0;
                messages_concat.push_back(message_concatenated);
                message_concatenated = std::to_string(group_by);
            }

        } else if ((group_num == num_concat) & (remainder > 0)) {

            if (msg_num == 0) {

                char str_remainder[3];
                sprintf(str_remainder, "%02lu", remainder);

                message_concatenated = std::string (str_remainder);
            } 

            if (msg_num == remainder - 1) { // don't add sep for last message to concat

                message_concatenated += message;
                messages_concat.push_back(message_concatenated);

            } else {

                message_concatenated += message + 's'; 
            }

            ++msg_num;
        }
    }

    messages_to_broadcast = messages_concat;
}

bool Broadcast::getConcat() const {
    return concat;
}

void Broadcast::addSentMessageLog(const std::string& msg) {

    std::string message_sent = 'b' + msg;

    std::lock_guard<std::mutex> lock(receiver_mutex);
    this->receiver->addMessageLog(message_sent);
}

void Broadcast::addDeliveredMessageLog(const std::string& msg) {

    std::string message_delivered(msg);
    message_delivered[0] = 'd';

    std::lock_guard<std::mutex> lock(receiver_mutex);
    this->receiver->addMessageLog(message_delivered);
}