cmake_minimum_required(VERSION 3.0.0)
project(lab1 VERSION 0.1.0)



set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(compiler_flags INTERFACE)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug...")
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
    set(gcc_like_cxx
    "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
    set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
    target_compile_options(compiler_flags INTERFACE
    "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
    "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
    )
else()
    message("not debug...")
endif()

add_custom_target(Txt_receiver SOURCES Txt.hpp)

add_executable(lab1 l1.cpp)
target_link_libraries(lab1 PUBLIC compiler_flags)

add_dependencies(lab1 Txt_receiver)

