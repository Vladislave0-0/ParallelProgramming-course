cmake_minimum_required(VERSION 3.12)
project(ThreadSafeCounter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

find_package(Threads REQUIRED)

add_executable(ThreadSafeCounter 
    src/main.cpp
    src/ThreadSafeCounter.cpp
)

target_link_libraries(ThreadSafeCounter 
    PRIVATE 
        Threads::Threads
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ThreadSafeCounter PRIVATE -Wextra)
endif()

set_target_properties(ThreadSafeCounter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

enable_testing()
add_test(NAME ThreadSafeCounterTest
         COMMAND ThreadSafeCounter
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
