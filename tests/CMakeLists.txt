project(Tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(TupleTest tuple_test.cpp)
target_link_libraries(TupleTest gtest gtest_main)
target_link_libraries(TupleTest tuple)

add_executable(ColorTest color_test.cpp)
target_link_libraries(ColorTest gtest gtest_main)
target_link_libraries(ColorTest color)

add_executable(CanvasTest canvas_test.cpp)
target_link_libraries(CanvasTest gtest gtest_main)
target_link_libraries(CanvasTest canvas)

add_executable(MatrixTest matrix_test.cpp)
target_link_libraries(MatrixTest gtest gtest_main)
target_link_libraries(MatrixTest matrix)

add_executable(RayTest ray_test.cpp)
target_link_libraries(RayTest gtest gtest_main)
target_link_libraries(RayTest ray)

add_executable(LightTest light_test.cpp)
target_link_libraries(LightTest gtest gtest_main)
target_link_libraries(LightTest light)

add_executable(MaterialTest material_test.cpp)
target_link_libraries(MaterialTest gtest gtest_main)
target_link_libraries(MaterialTest material)

add_executable(SphereTest sphere_test.cpp)
target_link_libraries(SphereTest gtest gtest_main)
target_link_libraries(SphereTest sphere)