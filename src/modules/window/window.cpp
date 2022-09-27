#include "window.hpp"
#include "modules/graphics/graphics.hpp"

namespace rosy::window {
    namespace {
        Arc<Window> s_window;
    }

    void Window::init(const std::string& title, int width, int height) {
        s_window = Arc<Window>::alloc();
        s_window->create(title, width, height);
    }

    auto Window::get() -> Arc<Window> const& {
        if (!s_window) {
            throw std::runtime_error("Window module is not initialized");
        }
        return s_window;
    }

    Window::Window() {
//        SDL_InitSubSystem(SDL_INIT_VIDEO);
        glfwInit();
    }

    Window::~Window() {
        glfwTerminate();
//        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    auto Window::create(const std::string& title, int width, int height) -> bool {
//        m_width = width;
//        m_height = height;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

//        m_window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (m_window == nullptr) {
//            throw std::runtime_error("Failed to open GLFW window");
            return false;
        }
//        m_context = SDL_GL_CreateContext(m_window);
//        if (m_context == nullptr) {
//            return false;
//        }
//
//        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//        SDL_GL_SetSwapInterval(0);
//
//        SDL_SetWindowResizable(m_window, SDL_TRUE);
//
//        gl3wInit();
//
        glfwMakeContextCurrent(m_window);
//        glfwGetFramebufferSize(m_window, &m_width, &m_height);

        return true;
    }

    void Window::destroy() {
//        SDL_GL_DeleteContext(m_context);
//        SDL_DestroyWindow(m_window);
//        m_context = nullptr;
        m_window = nullptr;
    }

    void Window::present() {
        glfwSwapBuffers(m_window);
//        SDL_GL_SwapWindow(m_window);
    }

    void Window::wrap_mouse() {
//        SDL_WarpMouseInWindow(m_window, m_width / 2, m_height / 2);
    }

    void Window::set_relative_mouse(bool relative) {
        m_is_mouse_relative = relative;

        if (relative) {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//            SDL_ShowCursor(SDL_DISABLE);
//            SDL_WarpMouseInWindow(m_window, m_width / 2, m_height / 2);
        } else {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

//            SDL_ShowCursor(SDL_ENABLE);
        }
    }

    auto Window::get_mouse_position() -> glm::vec2 {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        return glm::vec2(static_cast<float>(x), static_cast<float>(y));
    }

    void Window::set_size(int width, int height) {
//        m_width = width;
//        m_height = height;
    }

    void Window::set_focus(bool focus) {
        glfwSetWindowAttrib(m_window, GLFW_FOCUSED, focus ? GLFW_TRUE : GLFW_FALSE);
//        m_focus = focus;

        if (m_is_mouse_relative) {
            if (focus) {
//                SDL_ShowCursor(SDL_DISABLE);
//                SDL_WarpMouseInWindow(m_window, m_width / 2, m_height / 2);
            } else {
//                SDL_ShowCursor(SDL_ENABLE);
            }
        }
    }
    auto Window::has_focus() const -> bool {
        return glfwGetWindowAttrib(m_window, GLFW_FOCUSED) == GLFW_TRUE;
    }

    auto Window::get_width() const -> int {
        int width = 0;
        glfwGetFramebufferSize(m_window, &width, nullptr);
        return width;
    }

    auto Window::get_height() const -> int {
        int height = 0;
        glfwGetFramebufferSize(m_window, nullptr, &height);
        return height;
    }

    auto Window::get_key(int32_t key) -> bool {
        return glfwGetKey(m_window, key);
    }

    auto Window::is_mouse_relative() const -> bool {
        return m_is_mouse_relative;
    }

    auto get_width() -> int {
        return Window::get()->get_width();
    }

    auto get_height() -> int {
        return Window::get()->get_height();
    }

    void set_relative_mouse(bool relative) {
        Window::get()->set_relative_mouse(relative);
    }
}