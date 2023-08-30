cmake_minimum_required(VERSION 3.7)
project(DisplayImage)

file(GLOB COMMON_LIB_SOURCES "common/*.cpp")
add_library(common STATIC ${COMMON_LIB_SOURCES})
		   
add_library(lib STATIC
			camera.cpp
			renderer.cpp
			)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(main source.cpp)
target_link_libraries(main
	${OpenCV_LIBS}
	lib
	common
	)
