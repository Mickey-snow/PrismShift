project(prsh)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(tracer)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
	${OpenCV_LIBS}
	raytracer
	ext::tinyobj
	ext::argparse
	)
