cmake_minimum_required(VERSION 3.7)
project(Raytracer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# External libraries
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(JSONCPP_LIB_DIR "External/jsoncpp")
include_directories(${JSONCPP_LIB_DIR})
file(GLOB JSONCPP_SOURCES "${JSONCPP_LIB_DIR}/lib_json/*.cpp")
add_library(jsoncpp STATIC ${JSONCPP_SOURCES})

# Build libraries
set(GLOBAL_INCLUDE_LIB_DIR "include")
include_directories(${GLOBAL_INCLUDE_LIB_DIR})
file(GLOB GLOBAL_INCLUDE_LIB_SOURCES "${GLOBAL_INCLUDE_LIB_DIR}/*.cpp")
add_library(global_include STATIC ${GLOBAL_INCLUDE_LIB_SOURCES})


set(TRACER_LIB_DIR "tracer")
file(GLOB_RECURSE RAYTRACER_LIB_SOURCE "${TRACER_LIB_DIR}/*.cpp")
list(FILTER RAYTRACER_LIB_SOURCE EXCLUDE REGEX "${TRACER_LIB_DIR}/test/*")
add_library(raylib STATIC
			${RAYTRACER_LIB_SOURCE}
			)
target_include_directories(raylib PUBLIC ${TRACER_LIB_DIR})
target_link_libraries(raylib
			${OpenCV_LIBS}
			global_include
			jsoncpp
			)



# Build main
add_executable(main source.cpp)
target_link_libraries(main
	${OpenCV_LIBS}
	raylib
	global_include
	)


#Build tests
enable_testing()

set(TRACER_TEST_DIR "${TRACER_LIB_DIR}/test/")
file(GLOB_RECURSE TRACER_TEST_SOURCE "${TRACER_TEST_DIR}/*.cpp")
add_executable(tracer_test ${TRACER_TEST_SOURCE})

set(GLOBAL_INCLUDE_LIB_TEST_DIR "${GLOBAL_INCLUDE_LIB_DIR}/test/")
file(GLOB_RECURSE GLOBAL_INCLUDE_LIB_TEST_SOURCE "${GLOBAL_INCLUDE_LIB_TEST_DIR}/*.cpp")
add_executable(global_include_test ${GLOBAL_INCLUDE_LIB_TEST_SOURCE})

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
	global_include
	jsoncpp
	)

target_include_directories(global_include_test PUBLIC ${GLOBAL_INCLUDE_LIB_DIR})
target_link_libraries(global_include_test
	GTest::gtest_main
	global_include
	)


include(GoogleTest)

gtest_discover_tests(tracer_test)
gtest_discover_tests(global_include_test)

add_test(test_all
	global_include_test
	tracer_test)
