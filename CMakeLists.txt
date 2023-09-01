cmake_minimum_required(VERSION 3.7)
project(Raytracer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w --std=c++20")

file(GLOB COMMON_LIB_SOURCES "common/*.cpp")
add_library(common STATIC ${COMMON_LIB_SOURCES})
		   
add_library(raylib STATIC
			camera.cpp
			renderer.cpp
			bvh.cpp
			)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(main source.cpp)
target_link_libraries(main
	${OpenCV_LIBS}
	raylib
	common
	)

file(GLOB TEST_SOURCE_FILES "test/*.cpp")
add_executable(all_tests ${TEST_SOURCE_FILES})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
target_link_libraries(all_tests
	common
	GTest::gtest_main
	raylib
	)
include(GoogleTest)
gtest_discover_tests(all_tests)
