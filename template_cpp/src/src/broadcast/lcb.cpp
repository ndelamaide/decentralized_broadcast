#include "lcb.hpp"

#include <string>
#include <cstring>
#include <map>
#include <list>
#include <thread>

LocalizedCausalBroadcast::LocalizedCausalBroadcast(Receiver* receiver, UniformReliableBroadcast* urb,
                         unsigned long NUM_PROCESSES, std::list<int> dependencies, bool log)
    : Broadcast(receiver, log), lsn(0), dependencies(dependencies)
    {
        this->urb = urb;
        this->urb->setUpperLayer(this);

        receiver_mutex.lock();
        rank = this->receiver->getProcessId();
        receiver_mutex.unlock();

        for (int i=1; i <= static_cast<int> (NUM_PROCESSES); ++i) {
            VC[i] = 0;
        }

        deliver_pending_thread = std::thread(&LocalizedCausalBroadcast::deliverPending, this);
    }

void LocalizedCausalBroadcast::startBroadcast() {

    this->setBroadcastActive();
    this->urb->setBroadcastActive();

    char this_process_id[4];
    sprintf(this_process_id, "%03d", rank);

    for (auto& message: messages_to_broadcast) {

        std::string W;

        for (auto& other_rank: dependencies) {

            char process_id[4];
            sprintf(process_id, "%03d", other_rank);

            W += "v" + std::string (process_id) + std::to_string(VC[other_rank]);
        }
        
        std::string W_to_send(W);

        W_to_send += "v" + std::string(this_process_id) + std::to_string(lsn);
        lsn += 1;

        this->addSentMessageLog(message);

        std::string message_to_send = message + W_to_send;

        std::cout << "lcb broadcasting " << message_to_send << std::endl;

        this->urb->broadcastMessage(message_to_send, bool (true));

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // FOR TESTING ONLY
    }

    messages_to_broadcast.clear();
}

void LocalizedCausalBroadcast::broadcastMessage(const std::string& msg) {

    std::string W;

    for (auto& other_rank: dependencies) {

        char process_id[4];
        sprintf(process_id, "%03d", other_rank);

        W += "v" + std::string (process_id) + std::to_string(VC[other_rank]);
    }

    char this_process_id[4];
    sprintf(this_process_id, "%03d", rank);

    W += 'v' + std::string (this_process_id) + std::to_string(lsn);
    lsn += 1;

    std::string message_to_send = msg + W;

    this->urb->broadcastMessage(message_to_send);
}

void LocalizedCausalBroadcast::deliver(const std::string& msg) {
    //msg should be process_id - message - W

    std::lock_guard<std::mutex> lock(pairs_mutex);

    VectorClock msg_vc = pairs[msg];

    if (msg_vc.empty()) {

        std::string vc_string = msg.substr(msg.find('v'), msg.size());
        pairs[msg] = this->constructVC(vc_string);
    }
}

void LocalizedCausalBroadcast::deliverPending() {

    while (true) {
        if (active) {

            std::lock_guard<std::mutex> lock(pairs_mutex);

            std::map<std::string, VectorClock >::iterator it = pairs.begin();

            while (it != pairs.end()) {

                if (isInferior(it->second)) {

                    std::string message = it->first;

                    int other_rank = std::stoi(message.substr(0, 3));

                    VC[other_rank] += 1;

                    if (log) {

                        std::string message_to_deliver = this->toMessageFormat(message.substr(0, message.find('v')));

                        this->addDeliveredMessageLog(message_to_deliver);
                    }

                    it = pairs.erase(it); // points to next element in map

                } else {

                    ++it;
                }
            }
        }
    }
}

std::string LocalizedCausalBroadcast::toMessageFormat(const std::string msg) {

    std::string message_sender = msg.substr(0, 3);
    std::string message_payload = msg.substr(3, msg.size());

    std::string ack = "0";
    std::string target = "000";

    std::string formated = ack + message_sender + target + message_payload;

    return formated;
}

VectorClock LocalizedCausalBroadcast::constructVC(const std::string vc_string) {

    VectorClock W;

    unsigned long idx = vc_string.find('v');

    while(idx != std::string::npos) {
        
        int process_id = std::stoi(vc_string.substr(idx + 1, 3));
        unsigned long idx_next = vc_string.find('v', idx + 4);
        int sn;

        if (idx_next != std::string::npos) {

            sn = std::stoi(vc_string.substr(idx + 4, idx_next - (idx + 4)));

        } else {

            sn = std::stoi(vc_string.substr(idx + 4, vc_string.size()));
        }

        W[process_id] = sn;
        idx = idx_next;
    }

    return W;
}


bool LocalizedCausalBroadcast::isInferior(const VectorClock& W){

    for (auto& elem: W) {

        int other_rank = elem.first;

        if (elem.second > VC[other_rank]) {

            return bool (false);
        }
    }

    return bool (true);
}

