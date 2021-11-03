#ifndef BEB_H
#define BEB_H

#include "receiver.hpp"
#include "perfectlink.hpp"

#include <list>
#include <string>
#include <vector>

/** @brief Implements a best effort broadcast */
class BestEffortBroadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     * @param links the list of perfect links to other processes
     */
    BestEffortBroadcast(Receiver* receiver, std::vector<Perfectlink*> links);

    ~BestEffortBroadcast();

    /** @brief Adds a message to list of messages to broadcast
     * @param msg the message to add
     */
    void addMessage(const std::string& msg);

    /** @brief Activates the broadcast
     */
    void setBroadcastActive();

    /** @brief Deactivates the broadcast
     */
    void setBroadcastInactive();

    /** @brief Starts broadcasting. Transmits the messages to broadcast
     *  to the perfect links.
     */
    void startBroadcast();

    private:

    Receiver* receiver;
    std::vector<Perfectlink*> links;

    bool active;
    std::list<std::string> messages_to_broadcast;
    
};


#endif