#ifndef THREADLIGHTS_HPP
#define THREADLIGHTS_HPP

#include <cstdint>
#include <mutex>
#include <optional>
#include <thread>

#include "Lights.hpp"

class ThreadLights : public Lights {
   public:
    void processInput(uint32_t input) override;

    ThreadLights() : m_thread{[this]() { this->run(); }} {}

    ~ThreadLights() override {
        interrupt();
        m_thread.join();
    };

    ThreadLights(ThreadLights const&) = delete;
    ThreadLights(ThreadLights&&) = delete;
    ThreadLights& operator=(ThreadLights const&) = delete;
    ThreadLights& operator=(ThreadLights&&) = delete;

   private:
    class Interrupted {};

    void run();
    void interrupt();
    uint32_t get();

    std::mutex m_mutex{};
    std::optional<uint32_t> m_input{};
    std::thread m_thread;
    bool m_interrupt{false};
};

#endif  // THREADLIGHTS_HPP
