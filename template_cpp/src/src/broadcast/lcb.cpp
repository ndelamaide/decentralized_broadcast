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

        // TODO @ REMOVE
        for (int i=1; i <= static_cast<int> (NUM_PROCESSES); ++i) {
            VC[i] = 0;
        }
    }

void LocalizedCausalBroadcast::startBroadcast() {

    this->setBroadcastActive();
    this->urb->setBroadcastActive();

    std::string W;

    for (auto& other_rank: dependencies) {

        char process_id[4];
        sprintf(process_id, "%03d", other_rank);

        W += "v" + std::string (process_id) + std::to_string(VC[other_rank]);
    }

    char this_process_id[4];
    sprintf(this_process_id, "%03d", rank);

    for (auto& message: messages_to_broadcast) {
        
        std::string W_to_send(W);

        W_to_send += "v" + std::string (this_process_id) + std::to_string(lsn);
        lsn += 1;

        this->addSentMessageLog(message);

        std::string message_to_send = message + W_to_send;

        this->urb->broadcastMessage(message_to_send, bool (true));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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

    pending_mutex.lock();
    if (!(std::find(pending.begin(), pending.end(), msg) != pending.end())) {
        
        pending.push_back(msg);

        std::string vc_string = msg.substr(msg.find('v'), msg.size());
        message_VC_pairs[msg] = this->constructVC(vc_string);
    }

    std::list<std::string> pair_to_delete;

    bool exists(true);

    while (exists) {

        bool found_inferior(false);

        for (auto& message_VC: message_VC_pairs) {

            if (isInferior(message_VC.second)) {
                
                found_inferior = true;

                std::string message = message_VC.first;

                pending.remove(message);
                pair_to_delete.push_back(message);

                int other_rank = std::stoi(message.substr(0, 3));

                VC[other_rank] += 1;

                if (log) {

                    std::string message_to_deliver = this->toMessageFormat(message.substr(0, message.find('v')));

                    this->addDeliveredMessageLog(message_to_deliver);
                }
            }
        }

        //Remove messages delivered from message_VC_pair
        for (auto& message: pair_to_delete) {
            message_VC_pairs.erase(message);
        }

        exists = found_inferior;
    }

    pending_mutex.unlock();

}

std::string LocalizedCausalBroadcast::toMessageFormat(const std::string msg) {

    std::string message_sender = msg.substr(0, 3);
    std::string message_payload = msg.substr(3, msg.size());

    std::string ack = "0";
    std::string target = "000";

    std::string formated = ack + message_sender + target + message_payload;

    return formated;
}

std::map<int, int> LocalizedCausalBroadcast::constructVC(const std::string vc_string) {

    std::map<int, int> W;

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


bool LocalizedCausalBroadcast::isInferior(const std::map<int, int>& W){

    for (auto& elem: W) {

        int other_rank = elem.first;

        if (elem.second > VC[other_rank]) {

            return bool (false);
        }
    }

    return bool (true);
}

