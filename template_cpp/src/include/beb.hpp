#ifndef BEB_H
#define BEB_H

#include "broadcast.hpp"

/** @brief Implements a best effort broadcast */
class BestEffortBroadcast: public Broadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     * @param log true if as to log the messages
     */
    BestEffortBroadcast(Receiver* receiver, bool log = false);

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
    
};


#endif