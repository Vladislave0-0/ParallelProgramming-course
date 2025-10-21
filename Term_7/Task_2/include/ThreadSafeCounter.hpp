#pragma once

#include <atomic>
#include <mutex>
#include <string>

class ThreadSafeCounter final {
  std::atomic<int> value_ = 0;
  const int max_value_;
  mutable std::mutex mutex_;
  mutable std::mutex cout_mutex_;

public:
  explicit ThreadSafeCounter(int max);

  ThreadSafeCounter(const ThreadSafeCounter &) = delete;
  ThreadSafeCounter &operator=(const ThreadSafeCounter &) = delete;

  bool increment();

  void reset();

  int getValue() const;

  int getMaxValue() const;

  void print(const std::string &message) const;

  ~ThreadSafeCounter() = default;
};
