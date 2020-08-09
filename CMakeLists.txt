cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(ray-tracing2 VERSION 1.0 LANGUAGES CXX)

# Pick the C++ standard to compile with.
# Abseil currently supports C++11, C++14, and C++17.
set(CMAKE_CXX_STANDARD 20)

#set(FOLLY_ROOT_DIR /Users/blanders/.brew)
#find_package(folly)
#include_directories(${FOLLY_INCLUDE_DIR})


find_package(fmt REQUIRED)

#set(absl_DIR external/abseil-cpp)
find_package(absl REQUIRED)

enable_testing()
add_subdirectory(tests)

add_library(raytrace_lib "")
target_sources(raytrace_lib PRIVATE
        bounding_box.cpp
        camera.cpp
        canvas.cpp
        color.cpp
        cube.cpp
        group.cpp
        intersection.cpp
        light.cpp
        material.cpp
        matrix.cpp
        pattern.cpp
        plane.cpp
        obj_file.cpp
        ray.cpp
        shape.cpp
        sphere.cpp
        timer.cpp
        triangle.cpp
        tuple.cpp
        world.cpp
)
target_link_libraries(raytrace_lib fmt::fmt ${FOLLY_LIBRARIES})

add_executable(raytrace1 raytrace1.cpp)
target_link_libraries(raytrace1 raytrace_lib)

add_executable(raytrace2 raytrace2.cpp)
target_link_libraries(raytrace2 raytrace_lib)

add_executable(raytrace3 raytrace3.cpp)
target_link_libraries(raytrace3 raytrace_lib)

add_executable(raytrace4 raytrace4.cpp)
target_link_libraries(raytrace4 raytrace_lib)

add_executable(raytrace5 raytrace5.cpp)
target_link_libraries(raytrace5 raytrace_lib)

add_executable(raytrace6 raytrace6.cpp)
target_link_libraries(raytrace6 raytrace_lib)

add_executable(raytrace7 raytrace7.cpp)
target_link_libraries(raytrace7 raytrace_lib)
