#pragma once

#include <memory>

template<typename T>
struct Arc final : std::shared_ptr<T> {
    static auto alloc(auto&&... args) -> Arc {
        return {std::make_shared<T>(std::forward<decltype(args)>(args)...)};
    }
};

template<typename T>
Arc(std::shared_ptr<T>) -> Arc<T>;