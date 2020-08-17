cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(ray-tracing2 VERSION 1.0 LANGUAGES CXX)

# Pick the C++ standard to compile with.
# Abseil currently supports C++11, C++14, and C++17.
set(CMAKE_CXX_STANDARD 20)

find_package(absl REQUIRED)
find_package(fmt REQUIRED)
find_package(folly REQUIRED)
find_package(gflags REQUIRED)
find_package(glog REQUIRED)
find_package(yaml-cpp REQUIRED)

include(external/oneTBB/cmake/TBBGet.cmake)
tbb_get(TBB_ROOT tbb_root CONFIG_DIR TBB_DIR)
find_package(TBB REQUIRED)

enable_testing()
add_subdirectory(tests)

add_library(raytrace_lib "")
target_sources(raytrace_lib PRIVATE
        core/bounding_box.cpp
        core/camera.cpp
        core/canvas.cpp
        core/color.cpp
        core/intersection.cpp
        core/light.cpp
        core/material.cpp
        core/matrix.cpp
        core/pattern.cpp
        external/minipbrt/minipbrt.cpp
        #importers/obj_file.cpp
        importers/yaml_file.cpp
        core/ray.cpp
        utils/timer.cpp
        core/tuple.cpp
        core/world.cpp
        shapes/cube.cpp
        shapes/group.cpp
        shapes/plane.cpp
        shapes/shape.cpp
        shapes/sphere.cpp
        shapes/triangle.cpp
)
target_link_libraries(raytrace_lib fmt::fmt ${FOLLY_LIBRARIES} ${YAML_CPP_LIBRARIES} ${TBB_IMPORTED_TARGETS})

add_executable(raytrace1 apps/raytrace1.cpp)
target_link_libraries(raytrace1 raytrace_lib)

add_executable(raytrace2 apps/raytrace2.cpp)
target_link_libraries(raytrace2 raytrace_lib)

add_executable(raytrace3 apps/raytrace3.cpp)
target_link_libraries(raytrace3 raytrace_lib)

add_executable(raytrace4 apps/raytrace4.cpp)
target_link_libraries(raytrace4 raytrace_lib)

add_executable(raytrace5 apps/raytrace5.cpp)
target_link_libraries(raytrace5 raytrace_lib)

add_executable(raytrace6 apps/raytrace6.cpp)
target_link_libraries(raytrace6 raytrace_lib)

add_executable(raytrace7 apps/raytrace7.cpp)
target_link_libraries(raytrace7 PUBLIC raytrace_lib)

add_executable(raytrace8 apps/raytrace8.cpp)
target_link_libraries(raytrace8 PUBLIC raytrace_lib)

add_executable(render apps/render.cpp)
target_link_libraries(render PUBLIC raytrace_lib)
