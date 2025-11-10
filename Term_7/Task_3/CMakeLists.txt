cmake_minimum_required(VERSION 3.12)
project(LockFreeQueue)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(queue_tests tests/main.cpp)
target_link_libraries(queue_tests PRIVATE pthread)

# Добавляем флаги для поддержки многопоточности
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(queue_tests PRIVATE -pthread)
endif()
