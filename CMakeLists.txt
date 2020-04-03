cmake_minimum_required(VERSION 3.10)
project(lockfree_stack)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include(ExternalProject)

ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/master.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest source_dir binary_dir)

add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/lib/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)


set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/lib/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

include_directories("${source_dir}/googletest/include"
                    "${source_dir}/googlemock/include"
                    include)

add_executable(stackdemo
${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp
)


add_executable(runUnitTests
${CMAKE_CURRENT_SOURCE_DIR}/test/unittests/main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/test/unittests/stack_test.cpp
)

target_link_libraries(runUnitTests
    libgtest
    libgmock
)

add_executable(runStressTests
${CMAKE_CURRENT_SOURCE_DIR}/test/stresstests/main.cpp
${CMAKE_CURRENT_SOURCE_DIR}/test/stresstests/stack_stress_test.cpp
)

target_link_libraries(runStressTests
    libgtest
    libgmock
)




