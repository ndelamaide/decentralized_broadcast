#ifndef LCB_H
#define LCB_H

#include "broadcast.hpp"
#include "urb.hpp"

#include <map>
#include <list>

typedef std::map<int, int> VectorClock;

/** @brief Implements Localized Causal Broadcast */
class LocalizedCausalBroadcast : public Broadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     * @param urb the uniform reliable broadast
     * @param NUM_PROCESSES number of processes in the system
     * @param dependencies list of localized dependecies of this process
     * @param log true if has to log messages into output file
     */
    LocalizedCausalBroadcast(Receiver* receiver, UniformReliableBroadcast* urb, unsigned long NUM_PROCESSES, std::list<int> dependencies, bool log = false);

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

    /** @brief Delivers pending messages if can deliver them
     * i.e W' <= VC
     */
    virtual void deliverPending();

    private:

    /** @brief Changes a pending message to the format of
     * messages sent : ack.sender.target.payload
     * @param msg the message to format
     * @return the formated message
     */
    std::string toMessageFormat(const std::string msg);

    /** @brief Constructs a Vector Clock from the VC clock in string form
     * @param vc_string the VC in string form
     * @return the Vector Clock or empty if the VC doesn't 
     * satistfy W' < V from the algo 3.15 of the book
     */
    VectorClock constructVC(const std::string vc_string);

    /** @brief Checks if W is inferior to the Vc of this process
     * @param W the vector clock to compare
     * @return True if W <= VC
     */
    bool isInferior(const VectorClock& W);


    UniformReliableBroadcast* urb;

    int lsn;
    int rank;

    std::list<int> dependencies;

    VectorClock VC; // Vector Clock

    std::map<std::string, VectorClock > pairs; //message Vector clock pairs

    std::thread deliver_pending_thread;

    std::mutex pairs_mutex;
};

#endif