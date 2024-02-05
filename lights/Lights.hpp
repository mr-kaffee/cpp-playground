#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include <cstdint>
#include <iostream>

/**
 * Example abstract class to demonstrate awaiting co-routine
 */
class Lights {
   public:
    virtual ~Lights() = default;

    /// Get current lights.
    [[nodiscard]] uint32_t getLights() const {
        return m_lights;
    }

    /**
     * Provide an input to be processed.
     *
     * The interpretation of the input depends on the internal state of the lights.
     *
     * In the INIT phase, there is a first input expected indicating the number of states. Then there shall be one input
     * describing each of these states.
     *
     * Once the INIT phase is complete, the lights are in run phase. Subsequent inputs are interpreted as state number
     * to be activated.
     */
    virtual void processInput(uint32_t input) = 0;

   protected:
    /// Set the current lights and print some debug message.
    void setLights(uint32_t lights) {
        std::cout << "Switching lights from " << m_lights << " to " << lights << "\n";
        m_lights = lights;
    }

   private:
    /// The current lights, initialized to `0`.
    uint32_t m_lights{0};
};

#endif  // LIGHTS_HPP
