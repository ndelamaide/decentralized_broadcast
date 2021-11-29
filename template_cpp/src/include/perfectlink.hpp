#ifndef PERFECTLINK_H
#define PERFECTLINK_H

#include "receiver.hpp"
#include "sender.hpp"
#include "threadsafequeue.hpp"
#include "threadsafelist.hpp"

#include <string>
#include <list>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>

class Broadcast;

/** @brief Implements a perfect link */
class Perfectlink {

    public:

    /** @brief The class constructor
     * @param receiver the receiver in the perfect link
     * @param sender object to send messages to another process
     * @param broadcast the broadcast this link belongs to
     */
    Perfectlink(Receiver* receiver, Sender* sender, Broadcast* broadcast);

    ~Perfectlink();

    /** @brief Adds a message to the sending queue
     * @param msg the message to add
     */
    void addMessage(const std::string& msg);

    /** @brief Adds messages to the sending queue
     * @param msg the list of messages to add
     */
    void addMessages(const std::list<std::string>& msgs);

    /** @brief Sets the vector of other perfectlinks
     * @param other_links the other links
     */
    void addOtherLinks(std::vector<Perfectlink*> other_links);

    /** @brief Activates the link
     */
    void setLinkActive();

    /** @brief Deactivates the link
     */
    void setLinkInactive();

    /** @brief Sends a message and adds it to the messages to send list
     * @param msg the message to send
     */
    void sendNewMessage(const std::string& msg);

    /** @brief Thread to send messages in send queue
     */
    void sendThreaded();

    /** @brief Thread to receive messages
     */
    void deliverThreaded();

    /** @brief Thread to send acks
     */
    void ackThreaded();

    /** @brief Adds ack to the list of acks to send
     * @param ack the ack to add
     */
    void addAck(const std::string& ack);

    /** @brief Removes a message from the messages to send
    * @param msg the message to remove
    */
    void removeMessage(const std::string& msg);

    /** @brief Add a message to relay to other processes
    * @param msg the message to relay
    */
    void addMessageRelay(const std::string& msg);
    
    private:

    Receiver* receiver;
    Sender* sender;
    Broadcast* broadcast;

    std::vector<Perfectlink*> other_links;

    int target_id;
    int this_process_id;

    std::atomic<bool> link_active;

    std::thread deliver_thread;
    std::thread send_thread;
    std::thread ack_thread;

    std::mutex receiver_mutex;
    std::mutex messages_to_send_mutex;
    std::mutex acks_to_send_mutex;
    std::mutex link_mutex;
    std::mutex broadcast_mutex;

    std::list<std::string> messages_to_send;
    std::list<std::string> acks_to_send;

    ThreadsafeList link_delivered;
};


#endif