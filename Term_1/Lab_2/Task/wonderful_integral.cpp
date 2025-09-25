//========================================================================
// Необходимо сделать программу которая параллельно вычислит определенный
// интеграл sin(1/x) в пределе от некого положительного действительного
// числа до некоторого бОльшего положительного действительного числа.

// 1) Необходимо сбалансировать решение - время выполнения задачи на всех
//    нитях должно быть одинаковым.
// 2) Шаги интегрирования для функции sin(1/x) должны быть динамическими.
// 3) Используйте библиотеку pthread.
//========================================================================

// g++ wonderful_integral.cpp -O3 -o wonderful_integral.x && ./wonderful_integral.x 

#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <pthread.h>
#include <thread>
#include <vector>

const int RUNS = 5;

static inline double f(double x) { return std::sin(1.0 / x); }

static inline double derivative_f(double x) {
  return std::cos(1.0 / x) * (-1.0 / (x * x));
}

// адаптивный шаг по скорости роста функции
static inline double adaptive_step(double x, double base_step) {
  return base_step / (1.0 + 10.0 * std::fabs(derivative_f(x)));
}

struct WorkerArgs {
  double a, b;         // границы интегрирования
  int total_intervals; // общее количество интервалов
  double base_step;    // начальный шаг
  std::atomic<int>
      *next_interval; // следующий индекс отрезка, который нужно посчитать
  double *result;     // указатель на общий сбор
  pthread_mutex_t *result_mutex; // мьютекс для общего сбора
};

void *worker_func(void *arg) {
  auto *args = static_cast<WorkerArgs *>(arg);

  double local_sum = 0.0;

  double a = args->a;
  double b = args->b;
  int total_intervals = args->total_intervals;
  auto &next_i = *(args->next_interval);

  while (true) {
    int i = next_i.fetch_add(1, std::memory_order_relaxed);
    if (i >= total_intervals)
      break;

    double x = a + (b - a) * i / total_intervals;
    double next_x = a + (b - a) * (i + 1) / total_intervals;
    double dx = next_x - x;

    double step = adaptive_step(x, dx);
    int sub_steps = std::max(1, int(std::ceil(dx / step)));
    double sub_dx = dx / sub_steps;

    for (int j = 0; j < sub_steps; ++j) {
      double sub_x = x + j * sub_dx;
      double mid = sub_x + sub_dx * 0.5;
      local_sum += f(mid) * sub_dx;
    }
  }

  // блокируем запись в общий результат
  pthread_mutex_lock(args->result_mutex);
  *args->result += local_sum;
  pthread_mutex_unlock(args->result_mutex);

  return nullptr;
}

double parallel_integrate(double a, double b, int num_threads,
                          int total_intervals) {
  WorkerArgs args;

  std::atomic<int> next_interval{0};
  pthread_mutex_t result_mutex;
  pthread_mutex_init(&result_mutex, nullptr);
  double result = 0.0;

  double base_step = (b - a) / total_intervals;

  args.a = a;
  args.b = b;
  args.total_intervals = total_intervals;
  args.base_step = base_step;
  args.next_interval = &next_interval;
  args.result = &result;
  args.result_mutex = &result_mutex;

  std::vector<pthread_t> threads(num_threads);
  for (int i = 0; i < num_threads; ++i)
    pthread_create(&threads[i], nullptr, worker_func, &args);

  for (auto &th : threads)
    pthread_join(th, nullptr);

  pthread_mutex_destroy(&result_mutex);
  return result;
}

int main() {
  double a = 0.01;
  double b = 5.0;
  int num_threads =
      std::thread::hardware_concurrency(); // у меня 16 логических ядер
  int total_intervals = 1'000'000;

  long long time_sum_ns = 0;
  double last_result = 0.0;

  for (int i = 0; i < RUNS; ++i) {
    auto t0 = std::chrono::high_resolution_clock::now();
    last_result = parallel_integrate(a, b, num_threads, total_intervals);
    auto t1 = std::chrono::high_resolution_clock::now();
    time_sum_ns +=
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
  }

  long long avg_ns = time_sum_ns / RUNS;

  std::cout << "Integral from " << a << " to " << b << " = " << last_result
            << std::endl;
  std::cout << "Average time = " << avg_ns / 100000000.0 << " s\n";
}
