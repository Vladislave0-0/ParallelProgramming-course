#pragma once

#include <cmath>
#include <vector>

struct Point {
  double x, y;
};

double polygon_area(const std::vector<Point> &pts);

double polygon_area_parallel(const std::vector<Point> &pts);

std::vector<Point> generate_large_random_polygon(int n,
                                                 double max_coord = 10000);
                                                 