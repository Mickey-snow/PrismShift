cmake_minimum_required(VERSION 3.7)
project(PrismShift CXX)


set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")

include_directories(${CMAKE_SOURCE_DIR}/src/tracer)
include_directories(${CMAKE_SOURCE_DIR}/external)
include_directories(${CMAKE_SOURCE_DIR}/external/boost_math/include)
include_directories(${CMAKE_SOURCE_DIR}/external/json/include)
include_directories(${CMAKE_SOURCE_DIR}/external/argparse/include)
include_directories(${CMAKE_SOURCE_DIR}/external/spdlog/include)

add_subdirectory(external)
add_subdirectory(src)
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
