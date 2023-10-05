project(unittest)

include_directories(${PROJECT_SOURCE_DIR})

enable_testing()

file(GLOB_RECURSE PROJECT_SOURCE_FILES "${PROJECT_SOURCE_DIR}/*-test.cpp")
add_executable(unit_test ${PROJECT_SOURCE_FILES})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

target_link_libraries(unit_test
	gtest_main
	gmock_main
	raytracer
	)


include(GoogleTest)

gtest_discover_tests(unit_test)

add_test(test_all
	unit_test)
