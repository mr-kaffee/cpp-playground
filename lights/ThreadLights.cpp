#include "ThreadLights.hpp"

#include <cstdint>
#include <iostream>
#include <mutex>
#include <vector>

void ThreadLights::interrupt() {
    if (!m_interrupt) {
        std::lock_guard<std::mutex> const lock{m_mutex};

        m_interrupt = true;
    }
}

uint32_t ThreadLights::get() {
    while (true) {
        std::lock_guard<std::mutex> const lock{m_mutex};

        // if a value is available, process it
        if (m_input.has_value()) {
            auto input = m_input.value();
            m_input.reset();
            return input;
        }

        // if no value and interrupted, throw
        if (m_interrupt) {
            throw Interrupted{};
        }
    }
}

void ThreadLights::processInput(uint32_t input) {
    while (true) {
        std::lock_guard<std::mutex> const lock{m_mutex};

        if (!m_input.has_value()) {
            m_input = {input};
            return;
        }
    }
}

void ThreadLights::run() {
    try {
        // INIT, part 0
        auto len = get();
        std::vector<uint32_t> lightsVec{};
        lightsVec.reserve(len);
        // INIT, part 1
        for (int i = 0; i < len; ++i) {
            lightsVec.push_back(get());
        }

        // RUN
        while (true) {
            auto input = get();
            if (input < lightsVec.size()) {
                setLights(lightsVec[input]);
            } else {
                std::cout << "Out of bounds: " << input << " >= " << lightsVec.size() << "\n";
            }
        }
    } catch (Interrupted) {
        std::cout << "Thread interrupted.\n";
    }
}
