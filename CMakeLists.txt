cmake_minimum_required(VERSION 3.5)
project(ray-tracing2 VERSION 1.0 LANGUAGES CXX)

# Pick the C++ standard to compile with.
# Abseil currently supports C++11, C++14, and C++17.
set(CMAKE_CXX_STANDARD 17)

enable_testing()
add_subdirectory(tests)
add_subdirectory(external/abseil-cpp)

add_library(canvas  canvas.cpp)
target_link_libraries(canvas  color)
target_link_libraries(canvas  absl::str_format)

add_library(color  color.cpp)
target_link_libraries(color tuple)

add_library(matrix matrix.cpp)
target_link_libraries(matrix tuple)

add_library(tuple  tuple.cpp)

add_library(ray ray.cpp)
target_link_libraries(ray tuple matrix)

add_library(intersection intersection.cpp)
target_link_libraries(intersection ray tuple)

add_library(light light.cpp)
target_link_libraries(light color tuple)

add_library(material material.cpp)
target_link_libraries(material color light)

add_library(sphere sphere.cpp)
target_link_libraries(sphere tuple matrix)

add_executable(my_exe main.cpp)
target_link_libraries(my_exe canvas color matrix tuple ray material)
