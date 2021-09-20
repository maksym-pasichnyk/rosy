#pragma once

#include <string>
#include <modules/timer/timer.h>

namespace rosy {
    struct Config {
        struct {
            std::string title = "ROSY";
            int width = 640;
            int height = 480;
        } window;
    };

    extern void conf(Config& c);
    extern void load();
    extern void unload();
    extern void draw();
    extern void focus(bool status);
    extern void update(timer::duration dt);
    extern void resize(int width, int height);
}