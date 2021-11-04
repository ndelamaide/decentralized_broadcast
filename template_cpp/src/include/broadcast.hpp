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
     */
    Broadcast(Receiver* receiver);

    virtual ~Broadcast();

    /** @brief Adds the list of perfect links
     * @param links_to_add the list of perfect links to other processes
     */
    virtual void addLinks(std::vector<Perfectlink*> links_to_add);

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

    /** @brief Starts broadcasting
     */
    virtual void startBroadcast() = 0;

    /** @brief Delivers a message
     * @param msg the message to deliver
     */
    virtual void deliver(const std::string& msg) = 0;

    protected:

    Receiver* receiver;
    std::vector<Perfectlink*> links;

    bool active;
    std::list<std::string> messages_to_broadcast;
    
};


#endif