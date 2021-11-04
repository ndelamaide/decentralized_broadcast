#ifndef BEB_H
#define BEB_H

#include "broadcast.hpp"

/** @brief Implements a best effort broadcast */
class BestEffortBroadcast: public Broadcast {

    public:

    /** @brief The class constructor
     * @param receiver the receiver (process) broadcasting
     * @param links the list of perfect links to other processes
     */
    BestEffortBroadcast(Receiver* receiver, std::vector<Perfectlink*> links);

    /** @brief Starts broadcasting. Transmits the messages to broadcast
     *  to the perfect links.
     */
    void startBroadcast() override;
    
};


#endif