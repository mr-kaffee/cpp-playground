#include <cstdint>
#include <iostream>

#include "Lights.hpp"

constexpr uint32_t OFF = 0;
constexpr uint32_t GREEN = 1;
constexpr uint32_t YELLOW = 2;
constexpr uint32_t RED = 4;

constexpr uint32_t S_OFF = 0;
constexpr uint32_t S_RED = 1;
constexpr uint32_t S_GREEN = 2;
constexpr uint32_t S_YELLOW = 3;
constexpr uint32_t S_RED_YELLOW = 4;
constexpr uint32_t S_LEN = 5;
constexpr uint32_t S_OUT_OF_BOUNDS = 99;

void initLights(Lights& lights) {
    lights.processInput(S_LEN);

    lights.processInput(OFF);
    lights.processInput(RED);
    lights.processInput(GREEN);
    lights.processInput(YELLOW);
    lights.processInput(RED | YELLOW);
}

void initAndUseLights(Lights& lights) {
    initLights(lights);

    lights.processInput(S_GREEN);
    lights.processInput(S_YELLOW);
    lights.processInput(S_OUT_OF_BOUNDS);
    lights.processInput(S_RED);
    lights.processInput(S_RED_YELLOW);
}

int main() {
    std::cout << "---------------------- [START] StateMachineLights --------------------\n";
    {
        StateMachineLights lights{};
        initAndUseLights(lights);
    }
    std::cout << "---------------------- [END] StateMachineLights ----------------------\n\n";

    std::cout << "---------------------- [START] CoRoutineLights -----------------------\n";
    {
        CoRoutineLights lights{};
        initAndUseLights(lights);
    }
    std::cout << "---------------------- [END] CoRoutineLights -------------------------\n\n";

    std::cout << "---------------------- [START] ThreadLights --------------------------\n";
    {
        ThreadLights lights{};
        initAndUseLights(lights);
    }
    std::cout << "---------------------- [END] ThreadLights ----------------------------\n\n";

    std::cout << "======================================================================\n";
}
