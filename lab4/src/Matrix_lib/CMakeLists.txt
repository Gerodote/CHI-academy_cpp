# add a header-only lib
add_library(Matrix_lib INTERFACE)
target_include_directories(Matrix_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)