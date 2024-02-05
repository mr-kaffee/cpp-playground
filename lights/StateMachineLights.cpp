#include "StateMachineLights.hpp"

#include <cstdint>
#include <iostream>

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
