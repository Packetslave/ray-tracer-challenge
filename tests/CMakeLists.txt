project(Tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Tests "")
target_sources(Tests PRIVATE
        test_common.cpp
        camera_test.cpp
        canvas_test.cpp
        color_test.cpp
        cube_test.cpp
        light_test.cpp
        material_test.cpp
        matrix_test.cpp
        obj_file_test.cpp
        pattern_test.cpp
        plane_test.cpp
        ray_test.cpp
        shape_test.cpp
        sphere_test.cpp
        triangle_test.cpp
        tuple_test.cpp
        world_test.cpp
)

target_link_libraries(Tests gtest gtest_main)
target_link_libraries(Tests raytrace_lib ${FOLLY_LIBRARIES})
