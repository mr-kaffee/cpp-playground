#include "Lights.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

void StateMachineLights::processInput(uint32_t input) {
    switch (m_state) {
        case 0:
            // INIT, part 0: receive number of lights
            m_len = input;
            m_lightsVec.reserve(m_len);
            m_state = 1;
            break;
        case 1:
            // INIT, part 1: receive lights
            m_lightsVec.push_back(input);
            if (m_len == m_lightsVec.size()) {
                m_state = 2;
            }
            break;
        default:
            // RUN: activate given lights
            if (input < m_lightsVec.size()) {
                setLights(m_lightsVec[input]);
            } else {
                std::cout << "Out of bounds: " << input << " >= " << m_lightsVec.size() << "\n";
            }
            break;
    }
}

void CoRoutineLights::processInput(uint32_t input) {
    m_input.set(input);
}

// NOLINTBEGIN: readability-static-accessed-through-instance (clang tidy seems to be distracted by co-routines)
/**
 * State variable (m_state) is replaced by implicit co-routine frame.
 *
 * Member variables (m_len, m_lightsVec) are replaced by local variables (len, lightsVec) - could also be member
 * variables.
 *
 * Input value (input) is replaced by awaitable (m_input)
 */
CoRoutineLights::Task CoRoutineLights::run() noexcept {
    // INIT, part 0
    auto len = co_await m_input;
    std::vector<uint32_t> lightsVec{};
    lightsVec.reserve(len);
    // INIT, part 1
    for (int i = 0; i < len; ++i) {
        lightsVec.push_back(co_await m_input);
    }

    // RUN
    while (true) {
        auto input = co_await m_input;
        if (input < lightsVec.size()) {
            setLights(lightsVec[input]);
        } else {
            std::cout << "Out of bounds: " << input << " >= " << lightsVec.size() << "\n";
        }
    }
}

// NOLINTEND: readability-static-accessed-through-instance