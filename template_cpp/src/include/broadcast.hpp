#ifndef BROADCAST_H
#define BROADCAST_H

#include "receiver.hpp"
#include "perfectlink.hpp"

#include <list>
#include <string>
#include <vector>

/** @brief Implements a broadcast */
class Broadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     * @param links the list of perfect links to other processes
     */
    Broadcast(Receiver* receiver, std::vector<Perfectlink*> links);

    virtual ~Broadcast();

    /** @brief Adds a message to list of messages to broadcast
     * @param msg the message to add
     */
    virtual void addMessage(const std::string& msg);

    /** @brief Activates the broadcast
     */
    virtual void setBroadcastActive();

    /** @brief Deactivates the broadcast
     */
    virtual void setBroadcastInactive();

    /** @brief Starts broadcasting.
     */
    virtual void startBroadcast() = 0;

    protected:

    Receiver* receiver;
    std::vector<Perfectlink*> links;

    bool active;
    std::list<std::string> messages_to_broadcast;
    
};


#endif