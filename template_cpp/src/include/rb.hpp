#ifndef RB_H
#define RB_H

#include "beb.hpp"
#include "threadsafelist.hpp"


/** @brief Implements reliable broadcast */
class ReliableBroadcast : public BestEffortBroadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     */
    ReliableBroadcast(Receiver* receiver);

    /** @brief Delivers a message
     * @param msg the message to deliver
     */
    virtual void deliver(const std::string& msg) override;

    /** @brief Relay (broadcasts) a message delivered
     * @param msg the message to relay
     */
    void relay(const std::string& msg);
};

#endif
