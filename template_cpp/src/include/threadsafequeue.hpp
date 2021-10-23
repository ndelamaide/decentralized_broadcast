#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <string>

/* 
Code copied from :
https://codetrips.com/2020/07/26/modern-c-writing-a-thread-safe-queue/
*/

class ThreadsafeQueue {

    public:

    ThreadsafeQueue();

    virtual ~ThreadsafeQueue();

    unsigned long size() const;

    std::optional<std::string> pop();

    void push(const std::string &item);

    private: 

    std::queue<std::string> queue_;
    mutable std::mutex mutex_;
};

#endif