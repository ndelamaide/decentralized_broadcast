#include "threadsafelist.hpp"

#include <mutex>
#include <optional>
#include <list>
#include <string>
#include <atomic>
#include <algorithm>


ThreadsafeList::ThreadsafeList(){
}

ThreadsafeList::~ThreadsafeList(){
}

unsigned long ThreadsafeList::size() const {

    std::lock_guard<std::mutex> lock(mutex_);

    return list_.size();
}

std::atomic<bool> ThreadsafeList::contains(const std::string &item) const {
    
    std::list<std::string>::iterator it;
    std::lock_guard<std::mutex> lock(mutex_);

    return (std::find(list_.begin(), list_.end(), item) != list_.end());
}

void ThreadsafeList::push_back(const std::string &item) {

    std::lock_guard<std::mutex> lock(mutex_);

    list_.push_back(item);
}

std::list<std::string> ThreadsafeList::getList() const {
    
    std::lock_guard<std::mutex> lock(mutex_);

    return list_;
}