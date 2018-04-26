cmake_minimum_required(VERSION 3.5)
project(MVector)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(
    Test
    test.cpp
    MVector.h
    Exceptions.h
)
target_link_libraries(Test ${GTEST_LIBRARIES} pthread)