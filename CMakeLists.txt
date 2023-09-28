cmake_minimum_required(VERSION 3.7)
project(PrismShift CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

# External libraries
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories("External")

set(TINYOBJ_LIB_DIR "External/tinyobj")
include_directories(${TINYOBJ_LIB_DIR})
file(GLOB_RECURSE TINYOBJ_SOURCES "${TINYOBJ_LIB_DIR}/*.cc")
add_library(tinyobj STATIC ${TINYOBJ_SOURCES})

# Build libraries
set(TRACER_LIB_DIR "src/tracer")
file(GLOB_RECURSE RAYTRACER_LIB_SOURCE "${TRACER_LIB_DIR}/*.cpp")
add_library(raylib STATIC
			${RAYTRACER_LIB_SOURCE}
			)
target_include_directories(raylib PUBLIC ${TRACER_LIB_DIR})
target_link_libraries(raylib
			${OpenCV_LIBS}
			tinyobj
			)



# Build main
add_executable(main src/source.cpp)
target_link_libraries(main
	${OpenCV_LIBS}
	raylib
	tinyobj
	)


#Build tests
enable_testing()

set(TEST_DIR "test/")
file(GLOB_RECURSE TEST_SOURCE "${TEST_DIR}/*.cpp")
add_executable(unit_test ${TEST_SOURCE})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

target_include_directories(unit_test PUBLIC ${TRACER_LIB_DIR})
target_link_libraries(unit_test
	GTest::gtest_main
	${OpenCV_LIBS}
	raylib
	)


include(GoogleTest)

gtest_discover_tests(unit_test)

add_test(test_all
	unit_test)
