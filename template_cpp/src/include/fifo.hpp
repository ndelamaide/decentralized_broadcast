#ifndef FIFO_H
#define FIFO_H

#include "urb.hpp"

#include <vector>
#include <list>

/** @brief Implements FIFO broadcast */
class Fifo : public UniformReliableBroadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     * @param NUM_PROCESSES number of processes in the system
     */
    Fifo(Receiver* receiver, unsigned long NUM_PROCESSES);

    /** @brief Starts broadcasting. Transmits the messages to broadcast
     *  to the perfect links.
     */
    virtual void startBroadcast() override;

    /** @brief Delivers a message
     * @param msg the message to deliver
     */
    void deliverFifo(const std::string& msg);

    /** @brief Delivers pending messages if can deliver them
     */
    virtual void deliverPending() override;

    private:

    /** @brief Adds a sent message to the log
     * @param msg the message to add
     */
    virtual void addSentMessageLog(const std::string& msg) override;

    int lsn;

    std::mutex next_mutex;
    std::vector<unsigned long> next;

    std::mutex pending_fifo_mutex;
    std::list<std::string> pending_fifo;
};

#endif