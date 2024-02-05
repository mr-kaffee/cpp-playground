#ifndef STATEMACHINELIGHTS_HPP
#define STATEMACHINELIGHTS_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include "Lights.hpp"

/**
 * Implementation of lights using a simple state machine.
 */
class StateMachineLights : public Lights {
   public:
    void processInput(uint32_t input) override;

   private:
    /// state machine's state
    uint32_t m_state{};

    /// number of light states
    size_t m_len{};
    /// vector that holds all light states
    std::vector<uint32_t> m_lightsVec;
};

#endif  // STATEMACHINELIGHTS_HPP
