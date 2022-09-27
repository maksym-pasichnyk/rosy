//
// Created by Maksym Pasichnyk on 27.09.2022.
//

#include "input.hpp"
#include "modules/window/window.hpp"

namespace rosy::input {
    namespace {
        Arc<Input> s_input;
    }

    void Input::init() {
        s_input = Arc<Input>::alloc();
    }

    auto Input::get() -> Arc<Input> const& {
        if (!s_input) {
            throw std::runtime_error("Input module is not initialized");
        }
        return s_input;
    }

    Input::Input() {
        m_current_state.fill(false);
    }

    void Input::update() {
        auto& window = rosy::window::Window::get();

        m_frame += 1;

        m_position = window->get_mouse_position();
        m_moved = m_position != m_previous;
        if (m_moved) {
            m_delta = m_position - m_previous;
            m_previous = m_position;
        }


        for (size_t i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; ++i) {
            m_previous_state.at(i) = m_current_state.at(i);
            m_current_state.at(i) = window->get_key(int32_t(i));
        }
    }

    auto Input::get_mouse_position() const -> glm::vec2 {
        return m_position;
    }

    auto Input::get_mouse_delta() const -> glm::vec2 {
        return m_delta;
    }

    void Input::reset(int x, int y) {
        m_moved = false;
        m_position.x = x;
        m_position.y = y;
        m_previous.x = x;
        m_previous.y = y;
        m_delta.x = 0;
        m_delta.y = 0;
    }

    auto Input::get_key_down(uint8_t key) const -> bool {
        return !m_previous_state[key] && m_current_state[key];
    }

    auto Input::get_key(uint8_t key) const -> bool {
        return m_previous_state[key] && m_current_state[key];
    }

    auto Input::get_key_up(uint8_t key) const -> bool {
        return m_previous_state[key] && !m_current_state[key];
    }

    auto Input::get_mouse_moved() const -> bool {
        return m_moved;
    }
}