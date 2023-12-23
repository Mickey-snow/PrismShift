project(raytracer)


file(GLOB_RECURSE PROJECT_SOURCE_FILES "${PROJECT_SOURCE_DIR}/*.cpp")
add_library(raytracer STATIC ${PROJECT_SOURCE_FILES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(raytracer
	${OpenCV_LIBS}
	ext::tinyobj)
