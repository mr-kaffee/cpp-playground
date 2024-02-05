#include "CoRoutineLights.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

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
