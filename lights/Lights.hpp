#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include <cassert>
#include <coroutine>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

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

/**
 * Implementation of lights using co-routines.
 *
 * This implementation is not thread-safe. It allows a sequential, yet non-blocking implementation of the lights
 * state-machine.
 *
 * When a new instance is constructed, the run method is invoked and runs until it waits for an input. Whenever a new
 * input is provided via the `processInput` method, the co-routine resumes and consumes the input in the thread of the
 * caller of `processInput`.
 *
 * The co-routine frame will probably get allocated on the heap.
 *
 * See see https://lewissbaker.github.io/2017/11/17/understanding-operator-co-await
 */
class CoRoutineLights : public Lights {
   public:
    /// Create a new co-routine lights instance and start its internal co-routine.
    CoRoutineLights() {
        run();
    }

    /// default destructor, this is here to satisfy the "rule of five"
    ~CoRoutineLights() override = default;

    /// Deleted copy constructor, type is not copyable.
    CoRoutineLights(CoRoutineLights const&) = delete;

    /// Deleted copy assignment, type is not copyable.
    CoRoutineLights& operator=(CoRoutineLights const&) = delete;

    /// Deleted move constructor, type is not movable. Running co-routine would refer to original object.
    CoRoutineLights(CoRoutineLights&&) = delete;

    /// This type is move-assignable, type is not movable. Running co-routine would refer to original object.
    CoRoutineLights& operator=(CoRoutineLights&&) = delete;

    /**
     * Provide an input.
     *
     * Must only be called when object is ready, i.e., `ready()` yields `true`.
     */
    void processInput(uint32_t input) override;

    /// Check whether object is ready to accept inputs
    [[nodiscard]] bool ready() const noexcept {
        return m_input.ready();
    }

   private:
    /// Return object created by invoking run method
    struct Task {
        struct promise_type {
            static Task get_return_object() noexcept {
                return {};
            }

            static std::suspend_never initial_suspend() noexcept {
                return {};
            }

            static std::suspend_never final_suspend() noexcept {
                return {};
            }

            void return_void() noexcept {}

            void unhandled_exception() noexcept {}
        };
    };

    /// Start the co-routine, proceed as inputs are provided through `processInput`
    Task run() noexcept;

    /// Input type is Awaitable and Awaiter object
    class Input {
       public:
        /// Constructor.
        Input() noexcept = default;

        /// Destructor. Destroys the internal co-routine handle, if it exists.
        ~Input() {
            if (m_awaitingCoroutine) {
                m_awaitingCoroutine.destroy();
            }
        }

        /// Deleted copy constructor, type is not copyable.
        Input(Input const&) = delete;
        /// Deleted copy assignment, type is not copyable.
        Input& operator=(Input const&) = delete;
        /// Deleted move constructor, type is not movable. Running co-routine would refer to original object.
        Input(Input&&) = delete;
        /// Deleted move constructor, type is not movable. Running co-routine would refer to original object.
        Input& operator=(Input&&) = delete;

        /// return false, type is never considered ready when co_await is called
        static bool await_ready() noexcept {
            return false;
        }

        /// return true, always suspend until new data is received
        bool await_suspend(std::coroutine_handle<> awaitingCoroutine) noexcept {
            assert(!m_awaitingCoroutine && "Second co-routine awaiting");
            m_awaitingCoroutine = awaitingCoroutine;
            return true;
        }

        /// return value on resume
        uint32_t await_resume() noexcept {
            // no-body waiting, re-set to avoid handle to un-suspended co-routine being used
            m_awaitingCoroutine = nullptr;
            return m_value;
        }

        /**
         * Set value and resume awaiting co-routine
         *
         * This must only be called, when there is a co-routine awaiting, i.e., `ready()` yields `true`
         */
        void set(uint32_t value) noexcept {
            assert(m_awaitingCoroutine && "No co-routine awaiting");
            m_value = value;
            m_awaitingCoroutine.resume();
        }

        /// Return true if there is a co-routine awaiting on this `Input`
        [[nodiscard]] bool ready() const noexcept {
            return static_cast<bool>(m_awaitingCoroutine);
        }

       private:
        /// the handle of the co-routine awaiting for input
        std::coroutine_handle<> m_awaitingCoroutine;
        /// the current input value
        uint32_t m_value{};
    };

    /// Awaitable/Awaiter object to send dato to co-routine
    Input m_input;
};

#endif  // LIGHTS_HPP
