#pragma once

#include <string>
#include <GLFW/glfw3.h>

#include "Arc.hpp"
#include "glm/vec2.hpp"

namespace rosy::window {
    struct Window {
        static void init(const std::string& title, int width, int height);
        static auto get() -> Arc<Window> const&;

    public:
        Window();
        ~Window();

        auto create(const std::string& title, int width, int height) -> bool;
        void destroy();
        void present();

        void wrap_mouse();

        void set_size(int width, int height);
        void set_focus(bool focus);
        auto has_focus() const -> bool;
        auto get_width() const -> int;
        auto get_height() const -> int;
        auto get_key(int32_t key) -> bool;

        // todo: move to mouse module
        void set_relative_mouse(bool relative);

        auto get_mouse_position() -> glm::vec2;

        // todo: move to mouse module
        auto is_mouse_relative() const -> bool;

    private:
//        int m_width;
//        int m_height;
//        bool m_focus = false;
        bool m_is_mouse_relative = false;
        GLFWwindow* m_window = nullptr;
    };

    extern auto get_width() -> int;
    extern auto get_height() -> int;

    // todo: move to mouse module
    extern void set_relative_mouse(bool relative);
}