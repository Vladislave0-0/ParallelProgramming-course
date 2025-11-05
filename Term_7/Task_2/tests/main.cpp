#include "../include/lockfree_queue.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

// Однопоточный сценарий.
void test_single_thread() {
  LockFreeQueue<int> queue;

  queue.push(1);
  queue.push(2);
  queue.push(3);

  int val;
  // Извлекаем элементы в порядке FIFO.
  assert(queue.pop(val) && val == 1);
  assert(queue.pop(val) && val == 2);
  assert(queue.pop(val) && val == 3);

  std::cout << "[PASS] Single-thread test" << std::endl;
}

// Тест с пустой очередью.
void test_empty_queue() {
  LockFreeQueue<int> queue;
  int val;

  // Проверяем, что pop возвращает false для пустой очереди.
  assert(!queue.pop(val));

  // Добавляем и сразу извлекаем элемент.
  queue.push(42);
  assert(queue.pop(val) && val == 42);

  // Снова проверяем пустую очередь.
  assert(!queue.pop(val));

  std::cout << "[PASS] Empty queue test" << std::endl;
}

// Многопоточный сценарий.
void test_multi_thread() {
  LockFreeQueue<int> queue;
  // Количество потоков-производителей и потоков-потребителей.
  const int num_threads = 8;
  // Количество операций, которое должен выполнить каждый поток.
  const int ops_per_thread = 10000;
  const int total_ops = num_threads * ops_per_thread;

  // Потоки, добавляющие элементы в очередь.
  std::vector<std::thread> producers;
  // Потоки, извлекающие элементы из очереди.
  std::vector<std::thread> consumers;

  // Атомарный счётчик для подсчёта общего количества успешно извлечённых
  // элементов.
  std::atomic<int> popped_count(0);

  for (int i = 0; i < num_threads; ++i) {
    producers.emplace_back([&queue, ops_per_thread, i]() {
      for (int j = 0; j < ops_per_thread; ++j)
        queue.push(i * ops_per_thread + j);
    });
  }

  for (int i = 0; i < num_threads; ++i) {
    consumers.emplace_back([&queue, &popped_count, ops_per_thread]() {
      int local_count = 0;
      int val;
      while (local_count < ops_per_thread) {
        if (queue.pop(val)) {
          // Успешно извлекли.
          ++popped_count;
          ++local_count;
        }

        // Если очередь временно пуста, уступаем управление другим потокам,
        // чтобы избежать активного ожидания и снизить нагрузку на CPU
        std::this_thread::yield();
      }
    });
  }

  // Ожидаем завершения всех потоков.
  for (auto &t : producers)
    t.join();
  for (auto &t : consumers)
    t.join();

  // Проверяем общее количество извлеченных элементов.
  assert(popped_count == total_ops);
  std::cout << "[PASS] Multi-thread test (" << total_ops << " operations)"
            << std::endl;
}

int main() {
  std::cout << "=== Starting Lock-Free Queue Tests ===" << std::endl;

  test_single_thread();
  test_empty_queue();
  test_multi_thread();

  std::cout << "=== All tests passed successfully ===" << std::endl;
  return 0;
}
