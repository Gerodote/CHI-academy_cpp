message(STATUS "Adding doctest submodule")
include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    doctest
    PREFIX ${CMAKE_BINARY_DIR}/doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(doctest source_dir)
set(DOCTEST_INCLUDE_DIR ${source_dir}/doctest CACHE INTERNAL "Path to include folder for doctest")

# Set up executable target
add_executable(tests_target
    test_priority_queue_with_messages.cpp
)

# Link libraries and include directories
target_link_libraries(tests_target PRIVATE libQueue)
target_include_directories(tests_target PRIVATE ${DOCTEST_INCLUDE_DIR})

# Add the test as a CTest test
add_test(
    NAME priority_queue_test
    COMMAND tests_target
)

add_executable(test_lab5 test_lab5.cpp)
target_link_libraries(test_lab5 PRIVATE libQueue Boost::log)

add_test(NAME lab5_test COMMAND test_lab5)

# Add a custom target that runs the tests using CTest
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>
    DEPENDS tests_target
)