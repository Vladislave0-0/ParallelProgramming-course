#include "../include/ThreadSafeCounter.hpp"
#include <iostream>

ThreadSafeCounter::ThreadSafeCounter(int max_value) : max_value_(max_value) {}

bool ThreadSafeCounter::increment() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (value_ >= max_value_)
    return false;

  return ++value_, true;
}

void ThreadSafeCounter::reset() {
  std::lock_guard<std::mutex> lock(mutex_);
  value_ = 0;
}

int ThreadSafeCounter::getValue() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return value_;
}

int ThreadSafeCounter::getMaxValue() const { return max_value_; }

void ThreadSafeCounter::print(const std::string &message) const {
  std::lock_guard<std::mutex> lock(cout_mutex_);
  std::cout << message << std::endl;
}
