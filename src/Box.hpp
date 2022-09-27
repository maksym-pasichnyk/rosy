#pragma once

#include <memory>

template<typename T>
struct Box final : std::unique_ptr<T> {
    static auto alloc(auto&&... args) -> Box {
        return {std::make_unique<T>(std::forward<decltype(args)>(args)...)};
    }
};

template<typename T>
Box(std::unique_ptr<T>) -> Box<T>;