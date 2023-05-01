cmake_minimum_required(VERSION 3.20.0)

# set(CMAKE_CXX_CLANG_TIDY clang-tidy)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


project(lab4 VERSION 0.1.0)


add_subdirectory(src)


add_executable(lab4 main.cpp)
target_link_libraries(lab4 Matrix_lib)
