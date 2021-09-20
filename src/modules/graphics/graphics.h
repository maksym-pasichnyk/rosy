#pragma once

#include "shader.h"
#include <vector>
#include <memory>

namespace rosy::graphics {
    struct Graphics {
    public:
        Graphics();
        ~Graphics();

        void clear() noexcept;
        void present() noexcept;

        auto new_shader(ShaderType type, std::initializer_list<const char*> sources) -> std::shared_ptr<Shader>;
    };

    extern auto new_shader(ShaderType type, std::initializer_list<const char*> sources) -> std::shared_ptr<Shader>;
}