#ifndef FIFO_H
#define FIFO_H

#include "broadcast.hpp"
#include "urb.hpp"

#include <vector>
#include <list>

/** @brief Implements FIFO broadcast */
class Fifo : public Broadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     * @param urb the uniform reliable broadast
     * @param NUM_PROCESSES number of processes in the system
     * @param log true if has to log messages into output file
     */
    Fifo(Receiver* receiver, UniformReliableBroadcast* urb, unsigned long NUM_PROCESSES, bool log = false);

    /** @brief Starts broadcasting. Transmits the messages to broadcast
     *  to the perfect links.
     */
    virtual void startBroadcast() override;

    /** @brief Broadcasts a message
     * @param msg the message to broadcast
     */
    virtual void broadcastMessage(const std::string& msg) override;

    /** @brief Delivers a message
     * @param msg the message to deliver
     */
    virtual void deliver(const std::string& msg) override;

    private:

    /** @brief Changes a pending message to the format of
     * messages sent : ack.sender.target.payload
     * @param msg the message to fromat
     * @return the formated message
     */
    std::string toMessageFormat(const std::string msg);

    UniformReliableBroadcast* urb;

    int lsn;

    std::mutex next_mutex;
    std::vector<unsigned long> next;

    std::mutex pending_mutex;
    std::list<std::string> pending;
};

#endif