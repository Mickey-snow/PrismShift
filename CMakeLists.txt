cmake_minimum_required(VERSION 3.7)
project(Raytracer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -O2 --std=c++20")

set(TRACER_LIB_DIR "tracer/")


file(GLOB COMMON_LIB_SOURCES "${TRACER_LIB_DIR}/common/*.cpp")
file(GLOB RAYTRACER_LIB_SOURCES "${TRACER_LIB_DIR}/*.cpp")
		   
add_library(raylib STATIC
			${COMMON_LIB_SOURCES}
			${RAYTRACER_LIB_SOURCES}
			)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(main source.cpp)
target_link_libraries(main
	${OpenCV_LIBS}
	raylib
	)

set(TRACER_TEST_DIR "tracer/test/")
file(GLOB TRACER_TEST_SOURCE_FILES "${TRACER_TEST_DIR}/*.cpp")
add_executable(tracer_test ${TRACER_TEST_SOURCE_FILES})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

target_include_directories(tracer_test PUBLIC ${TRACER_LIB_DIR})
target_link_libraries(tracer_test
	GTest::gtest_main
	${OpenCV_LIBS}
	raylib
	)
include(GoogleTest)
gtest_discover_tests(tracer_test)
