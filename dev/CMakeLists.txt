cmake_minimum_required(VERSION 3.7)
project(coord)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
include_directories("src")

file(GLOB_RECURSE COORD_SRC "src/*.cpp")
add_library(coord STATIC ${COORD_SRC})

file(GLOB_RECURSE TEST_SRC "test/*.cpp")
add_executable(main_test ${TEST_SRC})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

target_link_libraries(main_test
	coord
	GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(main_test)

add_test(test_all main_test)
