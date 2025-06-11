project(prsh)

add_subdirectory(tracer)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        raytracer
        ext::tinyobj
        ext::argparse
        )
