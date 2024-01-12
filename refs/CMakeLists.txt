add_library(foo STATIC Foo.cpp)
add_executable(refs refs.cpp)
target_link_libraries(refs foo)

add_executable(simple simple.cpp)
