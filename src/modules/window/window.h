#pragma once

#include <SDL.h>
#include <string>

namespace rosy::window {
    struct Window {
    public:
        Window();
        ~Window();

        auto create(const std::string& title, int width, int height) -> bool;
        void destroy();
        void present();

        void wrap_mouse();

        void set_size(int width, int height) {
            m_width = width;
            m_height = height;
        }

        void set_focus(bool focus) {
            m_focus = focus;

            if (m_is_mouse_relative) {
                if (focus) {
                    SDL_ShowCursor(SDL_DISABLE);
                    SDL_WarpMouseInWindow(m_window, m_width / 2, m_height / 2);
                } else {
                    SDL_ShowCursor(SDL_ENABLE);
                }
            }
        }

        auto has_focus() const -> bool {
            return m_focus;
        }

        auto get_width() const -> int {
            return m_width;
        }

        auto get_height() const -> int {
            return m_height;
        }

        // todo: move to mouse module
        void set_relative_mouse(bool relative);

        // todo: move to mouse module
        auto is_mouse_relative() const -> bool {
            return m_is_mouse_relative;
        }
    private:
        int m_width;
        int m_height;
        bool m_focus = false;
        bool m_is_mouse_relative = false;
        SDL_Window *m_window = nullptr;
        SDL_GLContext m_context = nullptr;
    };

    extern auto get_width() -> int;
    extern auto get_height() -> int;

    // todo: move to mouse module
    extern void set_relative_mouse(bool relative);
}