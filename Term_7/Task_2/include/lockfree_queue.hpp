#pragma once

#include <atomic>

template <typename Ty> class LockFreeQueue {
private:
  struct Node {
    Ty data;
    std::atomic<Node *> next;

    explicit Node(Ty val) : data(std::move(val)), next(nullptr) {}
  };

  // Атомарные указатели на начало и конец очереди. alignas используем для
  // избежания `false sharing`.
  alignas(64) std::atomic<Node *> head;
  alignas(64) std::atomic<Node *> tail;

public:
  LockFreeQueue() {
    // Инициализируем очередь dummy-узлом.
    Node *dummy = new Node(Ty());
    head.store(dummy, std::memory_order_relaxed);
    tail.store(dummy, std::memory_order_relaxed);
  }

  ~LockFreeQueue() {
    while (Node *old_head = head.load(std::memory_order_relaxed)) {
      head.store(old_head->next, std::memory_order_relaxed);
      delete old_head;
    }
  }

  LockFreeQueue(const LockFreeQueue &) = delete;
  LockFreeQueue &operator=(const LockFreeQueue &) = delete;

  // Добавление элемента в конец очереди.
  void push(Ty value) {
    Node *new_node = new Node(std::move(value));

    while (true) {
      Node *current_tail = tail.load(std::memory_order_relaxed);
      Node *next = current_tail->next.load(std::memory_order_acquire);

      // Проверяем согласованность текущего состояния, так как за время между
      // первым чтением tail и чтением next, другой поток мог обновить tail.
      if (current_tail == tail.load(std::memory_order_relaxed)) {
        // Если current_tail - действительно конец очереди.
        if (next == nullptr) {
          // Пытаемся добавить новый узел в конец.
          if (current_tail->next.compare_exchange_strong(
                  next, new_node, std::memory_order_release,
                  std::memory_order_relaxed)) {
            // Успешно добавили узел. Обновляем tail, что, вообще говоря, не
            // всегда должно получаться, потому что другой поток уже мог
            // заменить tail, но нас это и не интересует с точки зрения
            // инварианта Майкла & Скотта. Главное, чтобы tail указывал хоть на
            // какой-то узел в цепочке, а мы попробуем это оптимизировать и
            // самостоятельно продвинуть tail.
            tail.compare_exchange_strong(current_tail, new_node,
                                         std::memory_order_release,
                                         std::memory_order_relaxed);
            return;
          }
        } else {
          // Если current_tail - не конец очереди. Значит элемент добавился в
          // другом потоке и мы продвинем tail.
          tail.compare_exchange_strong(current_tail, next,
                                       std::memory_order_release,
                                       std::memory_order_relaxed);
        }
      }
    }
  }

  // Извлекаем элемент из начала очереди.
  bool pop(Ty &result) {
    while (true) {
      Node *current_head = head.load(std::memory_order_relaxed);
      Node *next = current_head->next.load(std::memory_order_acquire);

      // Проверяем согласованность текущего состояния.
      if (current_head == head.load(std::memory_order_relaxed)) {
        // Если очередь пуста, вернём false.
        if (next == nullptr)
          return false;

        // Пытаемся продвинуть head
        if (head.compare_exchange_strong(current_head, next,
                                         std::memory_order_release,
                                         std::memory_order_relaxed)) {
          // Извлекли элемент. Сохраняем значение по ссылке.
          result = std::move(next->data);
          delete current_head;
          return true;
        }
      }
    }
  }
};
