#include "polygon_area.hpp"
#include <chrono>
#include <iostream>

int main(int argc, char **argv) {
  int n;
  std::cin >> n; // Введите количество вершин

  std::vector<Point> polygon(n);
  for (int i = 0; i < n; ++i)
    std::cin >> polygon[i].x >> polygon[i].y; // Введите координаты вершин (x, y)

  std::cout << "Площадь многоугольника: " << polygon_area(polygon) << "\n";
  std::cout << "Площадь многоугольника (OMP): " << polygon_area_parallel(polygon) << "\n\n";

  // int num = atoi(argv[1]);
  // std::cin >> num; // Введите количество вершин

  // std::vector<Point> polygon_new = generate_large_random_polygon(num);

  // auto t1 = std::chrono::high_resolution_clock::now();
  // double s1 = polygon_area(polygon_new);
  // auto t2 = std::chrono::high_resolution_clock::now();
  // std::chrono::duration<double> dur1 = t2 - t1;

  // auto t3 = std::chrono::high_resolution_clock::now();
  // double s2 = polygon_area_parallel(polygon_new);
  // auto t4 = std::chrono::high_resolution_clock::now();
  // std::chrono::duration<double> dur2 = t4 - t3;

  // std::cout << num << "\t" << dur1.count() << "\t" << dur2.count() << std::endl;
}
