#include "../include/ThreadSafeCounter.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

void testBasicOperations() {
  std::cout << "1. Testing basic operations: ";

  ThreadSafeCounter counter(5);

  // Trying simple test.
  for (int i = 0; i < 5; ++i) {
    bool status = counter.increment();
    assert(status == true);
    assert(counter.getValue() == i + 1);
  }

  // Trying to exceed maximum value.
  bool status = counter.increment();
  assert(status == false);
  assert(counter.getValue() == 5);

  // Trying reset.
  counter.reset();
  assert(counter.getValue() == 0);

  // Trying after reset.
  status = counter.increment();
  assert(status == true);
  assert(counter.getValue() == 1);

  std::cout << "passed" << std::endl;
}

void testMultithreadedAccess() {
  std::cout << "2. Testing multithreaded access: ";

  const int MAX_VALUE = 100;
  const int NUM_THREADS = 10;
  const int ATTEMPTS_PER_THREAD = 20;

  ThreadSafeCounter counter(MAX_VALUE);
  std::vector<std::thread> threads;
  std::atomic<int> successful_increments{0};

  // Start threads.
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([&]() {
      for (int j = 0; j < ATTEMPTS_PER_THREAD; ++j) {
        if (counter.increment())
          ++successful_increments;

        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
    });
  }

  // Awaiting completion.
  for (auto &t : threads)
    t.join();

  // Checking results.
  assert(counter.getValue() == MAX_VALUE);
  assert(successful_increments == MAX_VALUE);

  std::cout << "passed" << std::endl;
}

void testPerformance() {
  std::cout << "3. Running performance test: ";

  const int MAX_VALUE = 10000;
  const int NUM_THREADS = 50;

  ThreadSafeCounter counter(MAX_VALUE);
  std::vector<std::thread> threads;

  auto start_time = std::chrono::high_resolution_clock::now();

  // We run a large number of threads.
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([&]() {
      while (counter.increment())
        ; // Continue to increase as long as possible.
    });
  }

  for (auto &t : threads)
    t.join();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      end_time - start_time);

  // We check that we have not exceeded the limit.
  assert(counter.getValue() == MAX_VALUE);

  std::cout << "passed" << std::endl;
}

void testConcurrentReadWrite() {
  std::cout << "4. Testing concurrent read/write: ";

  ThreadSafeCounter counter(50);
  std::vector<std::thread> writers;
  std::vector<std::thread> readers;
  std::atomic<bool> stop{false};
  std::atomic<int> read_operations{0};

  const int NUM_WRITERS = 3;
  const int NUM_READERS = 2;

  // Writers.
  for (int i = 0; i < NUM_WRITERS; ++i) {
    writers.emplace_back([&]() {
      while (!stop.load()) {
        counter.increment();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    });
  }

  // Readers.
  for (int i = 0; i < NUM_READERS; ++i) {
    readers.emplace_back([&]() {
      while (!stop.load()) {
        assert(counter.getValue() <= counter.getMaxValue());

        ++read_operations;

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
      }
    });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  stop.store(true);

  for (auto &t : writers)
    t.join();

  for (auto &t : readers)
    t.join();

  std::cout << "passed" << std::endl;
}

void testThreadSafety() {
  std::cout << "5. Testing thread safety: ";

  const int MAX_VALUE = 500;
  const int NUM_THREADS = 20;
  const int ITERATIONS = 10;

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    ThreadSafeCounter counter(MAX_VALUE);
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
      threads.emplace_back([&]() {
        for (int j = 0; j < 100; ++j) {
          counter.increment();
          counter.getValue();

          if (j % 10 == 0)
            counter.reset();
        }
      });
    }

    for (auto &t : threads)
      t.join();

    assert(counter.getValue() >= 0 && counter.getValue() <= MAX_VALUE);
  }

  std::cout << "passed" << std::endl;
}

int main() {
  std::cout << "=== ThreadSafeCounter Test Suite ===\n\n";

  try {
    testBasicOperations();
    testMultithreadedAccess();
    testPerformance();
    testConcurrentReadWrite();
    testThreadSafety();

    std::cout << "\n=== ALL TESTS PASSED ===" << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "\n=== TEST FAILED ===" << std::endl;
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }
}
