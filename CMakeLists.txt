cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(ray-tracing2 VERSION 1.0 LANGUAGES CXX)

# Pick the C++ standard to compile with.
# Abseil currently supports C++11, C++14, and C++17.
set(CMAKE_CXX_STANDARD 17)

#set(FOLLY_ROOT_DIR /Users/blanders/.brew)
#find_package(folly)
#include_directories(${FOLLY_INCLUDE_DIR})

find_package(fmt REQUIRED)

#set(absl_DIR external/abseil-cpp)
find_package(absl REQUIRED)

enable_testing()
add_subdirectory(tests)

add_library(camera camera.cpp)
target_link_libraries(camera canvas fmt::fmt shape sphere)

add_library(canvas canvas.cpp)
target_link_libraries(canvas color fmt::fmt) # ${folly_LIBRARIES})

add_library(color color.cpp)
target_link_libraries(color tuple)

add_library(intersection intersection.cpp)
target_link_libraries(intersection ray matrix)

add_library(light light.cpp)
target_link_libraries(light color tuple)

add_library(material material.cpp)
target_link_libraries(material color light)

add_library(matrix matrix.cpp)
target_link_libraries(matrix tuple absl::flat_hash_map)

add_library(plane plane.cpp)
target_link_libraries(plane shape)

add_library(ray ray.cpp)
target_link_libraries(ray matrix)

add_library(shape shape.cpp)
target_link_libraries(shape intersection)

add_library(sphere sphere.cpp)
target_link_libraries(sphere shape)

add_library(world world.cpp)
target_link_libraries(world camera canvas intersection light ray shape)

add_library(timer timer.cpp)

add_library(tuple tuple.cpp)

add_executable(raytrace1 raytrace1.cpp)
target_link_libraries(raytrace1 camera canvas color intersection light material matrix ray sphere timer tuple )#${FOLLY_LIBRARIES})

add_executable(raytrace2 raytrace2.cpp)
target_link_libraries(raytrace2 canvas color intersection light material matrix ray sphere timer tuple )#${FOLLY_LIBRARIES})

add_executable(raytrace3 raytrace3.cpp)
target_link_libraries(raytrace3 camera canvas color intersection light material matrix ray sphere timer tuple )#${FOLLY_LIBRARIES})

add_executable(raytrace4 raytrace4.cpp)
target_link_libraries(raytrace4 camera canvas color intersection light material matrix ray plane sphere timer tuple )#${FOLLY_LIBRARIES})

add_executable(raytrace5 raytrace5.cpp)
target_link_libraries(raytrace5 camera canvas color intersection light material matrix ray plane sphere timer tuple )#${FOLLY_LIBRARIES})
