#pragma once

#include <chrono>

#include "Arc.hpp"

namespace rosy::timer {
    using clock = std::chrono::high_resolution_clock;
    using point = clock::time_point;
    using duration = std::chrono::duration<long, std::nano>;

    struct Timer {
        static void init();
        static auto get() -> Arc<Timer> const&;

        void reset() {
            m_step = duration(0);
            m_time = clock::now();
        }

        auto step() -> duration {
            m_frames++;
            m_last = m_time;

            m_time = clock::now();
            m_step = m_time - m_last;

            const auto time_since_last = std::chrono::duration<double>(m_time - m_prev_fps_update).count();
            if (time_since_last > 1.0) {
                m_fps = static_cast<size_t>(static_cast<double>(m_frames) / time_since_last);
                m_delta = time_since_last / m_frames;
                m_prev_fps_update = m_time;
                m_frames = 0;
            }
            return m_step;
        }

        auto get_time() const -> point {
            return clock::now();
        }

        auto get_fps() const -> size_t {
            return m_fps;
        }

        auto get_delta() const -> double {
            return m_delta;
        }

    private:
        point m_time = clock::now();
        point m_prev_fps_update = m_time;
        point m_last = m_time;
        duration m_step = duration(0);
        size_t m_frames{};
        size_t m_fps{};
        double m_delta{};
    };

    extern void sleep(const std::chrono::milliseconds& delay);
    extern auto get_time() -> point;
    extern auto get_fps() -> size_t;
    extern auto get_delta() -> double;
}