#include <rosy.hpp>

#include "modules/input/input.hpp"
#include "modules/event/event.hpp"
#include "modules/timer/timer.hpp"
#include "modules/audio/audio.hpp"
#include "modules/window/window.hpp"
#include "modules/graphics/graphics.hpp"

namespace rosy {
    auto boot(int argc, char **argv) -> int {
        using std::chrono::duration;
        using std::chrono::milliseconds;
        using rosy::timer::sleep;

        Config c{};
        conf(c);

        rosy::window::Window::init(c.window.title, c.window.width, c.window.height);
        rosy::graphics::Graphics::init();
        rosy::input::Input::init();
        rosy::timer::Timer::init();
        rosy::audio::Audio::init();

        auto timer = rosy::timer::Timer::get();
        auto input = rosy::input::Input::get();
        auto window = rosy::window::Window::get();
        auto graphics = rosy::graphics::Graphics::get();

        load();

        bool running = true;

        timer->reset();
        while (running) {
            glfwPollEvents();
//            while (auto event = rosy::event::poll()) {
////                if (event->type == SDL_QUIT) {
////                    running = false;
////                } else if (event->type == SDL_WINDOWEVENT) {
////                    const auto data = event->window;
////                    if (data.event == SDL_WINDOWEVENT_TAKE_FOCUS) {
////                        m_window->set_focus(true);
////                        if (m_window->is_mouse_relative()) {
////                            m_input->reset(m_window->get_width() >> 1, m_window->get_height() >> 1);
////                        }
////                        focus(true);
////                    } else if (data.event == SDL_WINDOWEVENT_FOCUS_LOST) {
////                        m_window->set_focus(false);
////                        focus(false);
////                    } else if (data.event == SDL_WINDOWEVENT_RESIZED) {
////                        m_window->set_size(data.data1, data.data2);
////                        resize(data.data1, data.data2);
////                    }
////                } else if (event->type == SDL_MOUSEMOTION) {
////                    if (m_window->has_focus()) {
////                        // todo: move to mouse module
////                        if (m_window->is_mouse_relative()) {
////                            m_window->wrap_mouse();
////                        }
////                        m_input->move(event->motion);
////                    }
////                    continue;
////                }
//            }
            auto dt = timer->step();
            input->update();

            update(dt);

            graphics->clear();
            draw();
            graphics->present();

            sleep(milliseconds(1));
        }

        unload();

        window->destroy();
        return 0;
    }
}

extern "C" auto main(int argc, char **argv) -> int {
    return rosy::boot(argc, argv);
}