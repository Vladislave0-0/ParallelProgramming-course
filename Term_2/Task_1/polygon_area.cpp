#include <random>
#include "polygon_area.hpp"

std::vector<Point> generate_large_random_polygon(int n,
                                                 double max_coord) {
  std::vector<Point> pts(n);
  std::mt19937 rng(42);
  std::uniform_real_distribution<double> dist(0.0, max_coord);

  for (int i = 0; i < n; i++)
    pts[i] = {dist(rng), dist(rng)};

  return pts;
}

double polygon_area(const std::vector<Point> &pts) {
  int n = pts.size();
  double area = 0.0;

  for (int i = 0; i < n; ++i) {
    int j = (i + 1) % n;
    area += pts[i].x * pts[j].y - pts[j].x * pts[i].y;
  }

  return std::fabs(area) / 2.0;
}

double polygon_area_parallel(const std::vector<Point> &pts) {
  int n = pts.size();
  double area = 0.0;

#pragma omp parallel for reduction(+ : area)
  for (int i = 0; i < n; ++i) {
    int j = (i + 1) % n;
    area += pts[i].x * pts[j].y - pts[j].x * pts[i].y;
  }

  return std::fabs(area) / 2.0;
}
