cmake_minimum_required(VERSION 3.14)
project(PolygonArea)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

find_package(GTest REQUIRED)

add_library(polygon_area_lib polygon_area.cpp polygon_area.hpp)

target_link_libraries(polygon_area_lib PRIVATE OpenMP::OpenMP_CXX)

add_executable(polygon_area main.cpp)
target_link_libraries(polygon_area PRIVATE polygon_area_lib)

enable_testing()
add_executable(google_test google_test.cpp)
target_link_libraries(google_test PRIVATE polygon_area_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(google_test)