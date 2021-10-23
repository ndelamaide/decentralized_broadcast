#ifndef THREADSAFELIST_H
#define THREADSAFELIST_H

#include <iostream>
#include <mutex>
#include <list>
#include <string>
#include <atomic>

class ThreadsafeList {

    public:

    ThreadsafeList();

    virtual ~ThreadsafeList();

    unsigned long size() const;

    std::atomic<bool> contains(const std::string &item) const;

    void push_back(const std::string &item);

    // Only to be used in main
    std::list<std::string> getList() const;

    private: 

    std::list<std::string> list_;
    mutable std::mutex mutex_;
};

#endif