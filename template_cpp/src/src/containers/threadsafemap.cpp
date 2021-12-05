#include "threadsafemap.hpp"

ThreadsafeMap::ThreadsafeMap() {
}

ThreadsafeMap::~ThreadsafeMap() {  
}

unsigned int& ThreadsafeMap::operator[](const std::string& string_) {
    std::lock_guard<std::mutex> lock(mutex_);
    return map_[string_];
}

