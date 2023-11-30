cmake_minimum_required(VERSION 3.12)

project(Playground VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(refs src/refs.cpp)

file(GLOB_RECURSE ALL_SOURCE_FILES src/*.cpp src/*.hpp)

add_custom_target(
    clang-tidy
    COMMAND /usr/bin/clang-tidy
    ${ALL_SOURCE_FILES}
    --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
    -p ${CMAKE_BINARY_DIR}
    --
    -std=c++20
    ${INCLUDE_DIRECTORIES}
)
