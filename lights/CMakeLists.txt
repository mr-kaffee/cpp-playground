add_library(Lights STATIC Lights.cpp)
add_executable(lights_app lights_app.cpp)
target_link_libraries(lights_app Lights)
