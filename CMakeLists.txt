cmake_minimum_required(VERSION 3.8)

project(lab5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -std=c++20")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

find_package(Boost 1.70 REQUIRED COMPONENTS log log_setup thread system filesystem)
find_package(SFML 2.5 REQUIRED COMPONENTS system audio)

if(Boost_FOUND AND SFML_FOUND)
    add_executable(lab5 src/server/server.cc src/server/main.cc)
    target_include_directories(lab5 PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(lab5 PRIVATE ${Boost_LIBRARIES} sfml-system sfml-audio)
else()
    message(FATAL_ERROR "Boost Asio, Boost Log and SFML are required but were not found. Please install them and try again.")
endif()
