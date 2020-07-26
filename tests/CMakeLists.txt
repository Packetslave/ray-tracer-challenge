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
