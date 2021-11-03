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

/** @brief Implements a perfect link */
class Perfectlink {

    public:

    /** @brief The class constructor
     * @param receiver the receiver in the perfect link
     * @param sender object to send messages to another process
     */
    Perfectlink(Receiver* receiver, Sender* sender);

    ~Perfectlink();

    /** @brief Adds a message to the sending queue
     * @param msg the message to add
     */
    void addMessage(const std::string& msg);

    /** @brief Adds messages to the sending queue
     * @param msg the list of messages to add
     */
    void addMessages(const std::list<std::string>& msgs);

    /** @brief Activates the link
     */
    void setLinkActive();

    /** @brief Deactivates the link
     */
    void setLinkInactive();

    /** @brief Thread to send messages in send queue
     */
    void sendThreaded();

    /** @brief Thread to receive messages
     */
    void deliverThreaded();

    /** @brief Gets the messages sent by this link
     * @return the set of messages sent
     */
    std::list<std::string> getMessagesSent() const;
    
    private:

    /** @brief Adds a sent message to the log
     * @param msg the message to add
     */
    void addSentMessageLog(const std::string& msg);

    /** @brief Adds a delivered message to the log
     * @param msg the message to add
     */
    void addDeliveredMessageLog(const std::string& msg);

    Receiver* receiver;
    Sender* sender;

    std::atomic<bool> link_active;

    std::thread deliver_thread;
    std::thread send_thread;

    std::mutex receiver_mutex;
    std::mutex pop_queue_mutex;
    
    std::atomic<bool> pop_queue;
    ThreadsafeQueue messages_to_send;
    ThreadsafeQueue ack_to_send;

    ThreadsafeList link_delivered; // Only use receiver delivered ?
    ThreadsafeList link_sent;

};


#endif