#include <gtest/gtest.h>
#include <chrono>

#include "polygon_area.hpp"

TEST(PolygonAreaTest, Triangle) {
  std::vector<Point> pts = {{0, 0}, {4, 0}, {0, 3}};
  EXPECT_DOUBLE_EQ(polygon_area(pts), 6.0);
}

TEST(PolygonAreaTest, Square) {
  std::vector<Point> pts = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  EXPECT_DOUBLE_EQ(polygon_area(pts), 1.0);
}

TEST(PolygonAreaTest, Rectangle) {
  std::vector<Point> pts = {{0, 0}, {5, 0}, {5, 2}, {0, 2}};
  EXPECT_DOUBLE_EQ(polygon_area(pts), 10.0);
}

TEST(PolygonAreaTest, Pentagon) {
  std::vector<Point> pts = {{0, 0}, {2, 0}, {3, 1}, {1.5, 2}, {0, 1}};
  EXPECT_DOUBLE_EQ(polygon_area(pts), 4);
}

TEST(PolygonAreaTest, NegativeCoords) {
  std::vector<Point> pts = {{-1, -1}, {2, -1}, {0, 2}};
  EXPECT_DOUBLE_EQ(polygon_area(pts), 4.5);
}

TEST(PolygonAreaTest, ClockwiseSquare) {
  std::vector<Point> pts = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
  EXPECT_DOUBLE_EQ(polygon_area(pts), 1.0);
}

static void test_big_data(size_t coords_num) {
  auto pts = generate_large_random_polygon(coords_num);

  auto t1 = std::chrono::high_resolution_clock::now();
  double s1 = polygon_area(pts);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> dur1 = t2 - t1;

  std::cout << "\n=====================================================\n";
  std::cout << "Количество координат: " << coords_num << std::endl;
  std::cout << "Однопоточная версия: " << dur1.count() << " сек\n";

  auto t3 = std::chrono::high_resolution_clock::now();
  double s2 = polygon_area_parallel(pts);
  auto t4 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> dur2 = t4 - t3;

  std::cout << "Многопоточная версия: " << dur2.count() << " сек\n";
  std::cout << "=====================================================\n\n";

  EXPECT_NEAR(s1, s2, 1e-3);
}

TEST(BigPolygonTest, Convex1e5) {
  test_big_data(100'000); // 100k вершин
}

TEST(BigPolygonTest, Convex1e6) {
  test_big_data(1'000'000); // 1M вершин
}

TEST(BigPolygonTest, Convex1e7) {
  test_big_data(10'000'000); // 10M вершин
}
