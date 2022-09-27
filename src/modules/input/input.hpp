#pragma once

#include <array>
#include <cstring>
#include <glm/vec2.hpp>
//#include <SDL_events.h>
//#include <SDL_keyboard.h>

#include "Arc.hpp"

namespace rosy::input {
    struct Input {
        static void init();
        static auto get() -> Arc<Input> const&;

    public:
        Input();

        void update();
        auto get_mouse_position() const -> glm::vec2;
        auto get_mouse_delta() const -> glm::vec2;
        void reset(int x, int y);

//        void move(const SDL_MouseMotionEvent& event)t {
//            m_previous = m_position;
//            m_position.x = event.x;
//            m_position.y = event.y;
//        }

        [[nodiscard]] auto get_key_down(uint8_t key) const -> bool;
        [[nodiscard]] auto get_key(uint8_t key) const -> bool;
        [[nodiscard]] auto get_key_up(uint8_t key) const -> bool;
        [[nodiscard]] auto get_mouse_moved() const -> bool;

    private:
//        uint8_t const* const m_keys = SDL_GetKeyboardState(nullptr);
        uint64_t m_frame = 0;

        std::array<bool, 512> m_current_state;
        std::array<bool, 512> m_previous_state;
        glm::vec2 m_position;
        glm::vec2 m_previous;
        glm::vec2 m_delta;
        bool m_moved = false;
        bool m_moved_update = false;
    };
}