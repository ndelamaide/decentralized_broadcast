#ifndef SENDER_H
#define SENDER_H

#include "udpclient.hpp"

#include <iostream>
#include <string>
#include <atomic>

/** @brief Implements a sender to another process */
class Sender: public UDPclient {

    public:

    /** @brief The class constructor
     * @param target_addr the adress to send messages to
     * @param target_id the id of the process to send messages to
     */
    Sender(sockaddr_in target_addr, int target_id);

    /** @brief Sends the message to send to target
     * @return ssize_t of message sent or not
     */
    ssize_t send();

    /** @brief Gets the target id
     * @return the target id id
     */
    int getTargetId() const;

    /** @brief Sets the message to send
     */
    void setMessageToSend(const std::string& msg);

    /** @brief Return the message to send to target
     * @return string of message to send
     */
    std::string getMessageToSend() const;

    /** @brief Sets the can_send boolean
     * @param bool_ the boolean
     */
    void setCanSend(bool bool_);

    private:

    int target_id;

    std::string msg_to_send;
    std::atomic<bool> can_send;

};

#endif