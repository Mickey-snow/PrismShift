project(raytracer)


file(GLOB_RECURSE PROJECT_SOURCE_FILES "${PROJECT_SOURCE_DIR}/*.cpp")
list(REMOVE_ITEM PROJECT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/renderer.cpp)
list(REMOVE_ITEM PROJECT_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/textures/imagetexture.cpp)
add_library(raytracer STATIC ${PROJECT_SOURCE_FILES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(raytracer
        ext::tinyobj)
