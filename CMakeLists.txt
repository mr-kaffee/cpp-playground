cmake_minimum_required(VERSION 3.12)

project(Playground VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(foo STATIC src/Foo.cpp)
add_executable(refs src/refs.cpp)
add_executable(simple src/simple.cpp)

target_link_libraries(refs foo)

add_custom_target(preproc_refs2
    COMMAND ${CMAKE_CXX_COMPILER} -Xclang -ast-dump -fsyntax-only ${CMAKE_CURRENT_SOURCE_DIR}/src/refs2.cpp > ${CMAKE_CURRENT_BINARY_DIR}/refs2_ast.txt
    DEPENDS refs2
    COMMENT "Generating preprocessed output"
)

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
