add_library(Lights STATIC StateMachineLights.cpp CoRoutineLights.cpp ThreadLights.cpp)
add_executable(lights_app lights_app.cpp)
target_link_libraries(lights_app Lights)
