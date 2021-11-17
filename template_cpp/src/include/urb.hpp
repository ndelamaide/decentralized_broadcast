#ifndef URB_H
#define URB_H

#include "broadcast.hpp"
#include "beb.hpp"

#include <map>
#include <list>
#include <string>
#include <set>
#include <thread>
#include <mutex>

/** @brief Implements uniform reliable broadcast */
class UniformReliableBroadcast : public Broadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     * @param beb the best effort broadcast
     * @param NUM_PROCESSES number of processes in the system
     * @param log true if has to log messages into output file
     */
    UniformReliableBroadcast(Receiver* receiver, BestEffortBroadcast* beb, unsigned long NUM_PROCESSES, bool log = false);

    /** @brief Activates the broadcast
     */
    virtual void setBroadcastActive() override;

    /** @brief Starts broadcasting. Transmits the messages to broadcast
     *  to the perfect links.
     */
    virtual void startBroadcast() override;

    /** @brief Broadcasts a message
     * @param msg the message to broadcast
     */
    virtual void broadcastMessage(const std::string& msg) override;

    /** @brief Delivers a message
     * @param msg the message to deliver
     */
    virtual void deliver(const std::string& msg) override;

    /** @brief Checks if can deliver a message
     * @param msg the message to deliver
     * @return true if can deliver false otherwise
     */
    bool canDeliver(const std::string& msg) const;

    /** @brief Delivers pending messages if can deliver them
     */
    virtual void deliverPending();


    private:

    BestEffortBroadcast* beb;

    /** @brief Changes a pending message to the format of
     * messages sent : ack.sender.target.payload
     * @param msg the message to fromat
     * @return the formated message
     */
    std::string toMessageFormat(const std::string msg);

    unsigned long NUM_PROCESSES;

    std::list<std::string> pending;
    std::multimap<std::string, std::string> ack;

    std::thread deliver_pending_thread;

    std::mutex pending_mutex;
};



#endif