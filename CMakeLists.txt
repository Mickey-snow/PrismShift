cmake_minimum_required(VERSION 3.7)
project(DisplayImage)

add_library(common STATIC
		   vector3.cpp
		   ray.cpp
		   camera.cpp
		   color.cpp
		   )
		   
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(main source.cpp)
target_link_libraries(main
	${OpenCV_LIBS}
	common
	)
