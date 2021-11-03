#ifndef RECEIVER_H
#define RECEIVER_H

#include "udpserver.hpp"
#include "threadsafelist.hpp"

#include <list>
#include <string>
#include <atomic>

/** @class Implements the receiver of a process */
class Receiver: public UDPserver {

    public:

    /** @brief The class constructor
     * @param ip of the receiver
     * @param port of the receiver
     * @param process_id of the receiver (process)
     */
    Receiver(in_addr_t ip, unsigned short port, int process_id);

    /** @brief Receives a message
     * @param buffer where message will be stored
     * @param ssize_t if message was receive or not
     */
    ssize_t receive(char* buffer) override;

    /** @brief Gets the process id
     * @return this process id
     */
    int getProcessId() const;

    /** @brief Sets the can_receive boolean
     * @param bool_ the boolean
     */
    void setCanReceive(bool bool_);

    /** @brief Adds a message to the set of delivered messages
     * @param msg the message to add
     */
    void addMessageDelivered(const std::string& msg);

    /** @brief Gets the messages delivered by this receiver (process)
     * @return the set of messages delivered
     */
    std::list<std::string> getMessagesDelivered() const;

    /** @brief Adds a message to the log
     * @param msg the message to add
     */
    void addMessageLog(const std::string& msg);

    /** @brief Gets the log of this receiver (process)
     * @return the list of messages delivered or sent
     */
    std::list<std::string> getMessageLog() const;


    private:

    int process_id;
    std::atomic<bool> can_receive;

    /** @brief Messages delivered by this receiver (process) */
    ThreadsafeList process_delivered;

    /** @brief Messages delivered or sent by this receiver (process) */
    ThreadsafeList process_log;

};


#endif