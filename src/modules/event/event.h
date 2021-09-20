#pragma once

#include <optional>
#include <SDL_events.h>

namespace rosy::event {
    using Event = SDL_Event;

    static auto poll() -> std::optional<Event> {
        Event event;
        if (SDL_PollEvent(&event)) {
            return event;
        }
        return std::nullopt;
    }

    static void pump() {
        SDL_PumpEvents();
    }
}