cmake_minimum_required(VERSION 3.8)

project(lab5_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -std=c++20")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

find_package(SFML 2.5 COMPONENTS audio REQUIRED)

if(SFML_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(lab5_test client.cc)
    target_link_libraries(lab5_test PRIVATE sfml-system sfml-audio)
else()
    message(FATAL_ERROR "SFML is required but was not found. Please install SFML and try again.")
endif()
