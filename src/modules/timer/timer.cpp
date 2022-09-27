#include "timer.hpp"

#include <thread>

namespace rosy::timer {
    namespace {
        Arc<Timer> s_timer;
    }

    void Timer::init() {
        s_timer = Arc<Timer>::alloc();
    }
    auto Timer::get() -> Arc<Timer> const& {
        if (!s_timer) {
            throw std::runtime_error("Timer module is not initialized");
        }
        return s_timer;
    }

    void sleep(const std::chrono::milliseconds& delay) {
        std::this_thread::sleep_for(delay);
    }

    auto get_time() -> point {
        return Timer::get()->get_time();
    }
    auto get_fps() -> size_t {
        return Timer::get()->get_fps();
    }
    auto get_delta() -> double {
        return Timer::get()->get_delta();
    }
}