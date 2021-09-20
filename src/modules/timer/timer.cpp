#include "timer.h"

#include <thread>
#include <module.h>

namespace rosy::timer {
    namespace {
        module<Timer> instance;
    }

    void sleep(const std::chrono::milliseconds& delay) {
        std::this_thread::sleep_for(delay);
    }

    auto get_time() -> point {
        return instance->get_time();
    }
    auto get_fps() -> size_t {
        return instance->get_fps();
    }
    auto get_delta() -> double {
        return instance->get_delta();
    }
}