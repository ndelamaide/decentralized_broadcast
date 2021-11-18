#ifndef BROADCAST_H
#define BROADCAST_H

#include "receiver.hpp"
#include "perfectlink.hpp"
#include <threadsafelist.hpp>

#include <list>
#include <string>
#include <vector>
#include <mutex>

/** @brief Implements a broadcast */
class Broadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     * @param log true if has to log messages into output file
     */
    Broadcast(Receiver* receiver, bool log = false);

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

    /** @brief Broadcasts a message
     * @param msg the message to broadcast
     */
    virtual void broadcastMessage(const std::string& msg) = 0;

    /** @brief Delivers a message
     * @param msg the message to deliver
     */
    virtual void deliver(const std::string& msg) = 0;

    /** @brief Sets the upper layer broadcast abstraction
     * @param upper_layer the upper layer
     */
    virtual void setUpperLayer(Broadcast* upper_layer);

    /** @brief Concatenates the messages to send
     */
    void concatMessages();

    /** @brief Gets the concat boolean
     * @return the boolean concat
     */
    bool getConcat() const;

    protected:

    /** @brief Adds a sent message to the log
     * @param msg the message to add
     */
    virtual void addSentMessageLog(const std::string& msg);

    /** @brief Adds a delivered message to the log
     * @param msg the message to add
     */
    void addDeliveredMessageLog(const std::string& msg);

    Receiver* receiver;
    std::vector<Perfectlink*> links;

    Broadcast* upper_layer;
    
    std::mutex upper_layer_mutex;
    std::mutex receiver_mutex;

    bool active;
    std::list<std::string> messages_to_broadcast;
    ThreadsafeList messages_delivered;

    bool log;
    bool concat;    
};


#endif