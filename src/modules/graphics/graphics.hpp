#pragma once

#include <vector>

#include "shader.hpp"
#include "Arc.hpp"

namespace rosy::window {
    struct Window;
}

namespace rosy::graphics {
    struct Graphics {
        static void init();
        static auto get() -> Arc<Graphics> const&;

    public:
        Graphics(Arc<rosy::window::Window> window);
        ~Graphics();

        void clear() noexcept;
        void present() noexcept;

        auto new_shader(ShaderType type, std::initializer_list<const char*> sources) -> std::shared_ptr<Shader>;

    private:
        Arc<rosy::window::Window> m_window;
    };

    extern auto new_shader(ShaderType type, std::initializer_list<const char*> sources) -> std::shared_ptr<Shader>;
}