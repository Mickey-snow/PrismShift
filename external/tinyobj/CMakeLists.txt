project(tinyobj)

add_library(${PROJECT_NAME} tiny_obj_loader.cc)
add_library(ext::tinyobj ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
					   ${PROJECT_SOURCE_DIR}
					   ${PROJECT_SOURCE_DIR}/mapbox)
