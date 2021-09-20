#include <rosy.h>
#include <module.h>
#include <modules/input/input.h>
#include <modules/event/event.h>
#include <modules/timer/timer.h>
#include <modules/window/window.h>
#include <modules/graphics/graphics.h>

namespace {
    module<rosy::graphics::Graphics> m_graphics;
    module<rosy::window::Window> m_window;
    module<rosy::timer::Timer> m_timer;
    module<rosy::input::Input> m_input;
}

namespace rosy {
    extern "C" auto main(int argc, char **argv) -> int {
        using std::chrono::duration;
        using std::chrono::milliseconds;
        using rosy::timer::sleep;

        m_timer->reset();

        Config c{};
        conf(c);

        m_window->create(c.window.title, c.window.width, c.window.height);

        load();

        m_timer->step();
        for (bool running = true; running;) {
            while (auto event = rosy::event::poll()) {
                if (event->type == SDL_QUIT) {
                    running = false;
                } else if (event->type == SDL_WINDOWEVENT) {
                    const auto data = event->window;
                    if (data.event == SDL_WINDOWEVENT_TAKE_FOCUS) {
                        m_window->set_focus(true);
                        if (m_window->is_mouse_relative()) {
                            m_input->reset(m_window->get_width() >> 1, m_window->get_height() >> 1);
                        }
                        focus(true);
                    } else if (data.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                        m_window->set_focus(false);
                        focus(false);
                    } else if (data.event == SDL_WINDOWEVENT_RESIZED) {
                        m_window->set_size(data.data1, data.data2);
                        resize(data.data1, data.data2);
                    }
                } else if (event->type == SDL_MOUSEMOTION) {
                    if (m_window->has_focus()) {
                        // todo: move to mouse module
                        if (m_window->is_mouse_relative()) {
                            m_window->wrap_mouse();
                        }
                        m_input->move(event->motion);
                    }
                    continue;
                }
            }
            auto dt = m_timer->step();
            m_input->update();

            update(dt);

            m_graphics->clear();
            draw();
            m_graphics->present();

            sleep(milliseconds(1));
        }

        unload();

        m_window->destroy();
        return 0;
    }
}