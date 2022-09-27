#include "graphics.hpp"
#include "fmt/format.h"

#include <GL/glew.h>
#include "modules/window/window.hpp"

#include "Arc.hpp"

namespace rosy::graphics {
    namespace {
        Arc<Graphics> s_graphics;

        bool check(const GLuint program) {
            GLint isLinked = 0;
            glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
            if (isLinked == GL_FALSE) {
                GLint maxLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
                fmt::print("{}\n", std::string_view(infoLog.data(), infoLog.size()));
                return false;
            }
            return true;
        }
    }

    void Graphics::init() {
        s_graphics = Arc<Graphics>::alloc(rosy::window::Window::get());
    }

    auto Graphics::get() -> Arc<Graphics> const& {
        if (!s_graphics) {
            throw std::runtime_error("Graphics module is not initialized");
        }
        return s_graphics;
    }

    Graphics::Graphics(Arc<rosy::window::Window> window) : m_window(std::move(window)) {
        glewExperimental = GL_TRUE;

        glewInit();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    Graphics::~Graphics() {}

    void Graphics::present() noexcept {
        m_window->present();
    }

    void Graphics::clear() noexcept {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    auto Graphics::new_shader(ShaderType type, std::initializer_list<const char*> sources) -> std::shared_ptr<Shader> {
        auto program = glCreateShaderProgramv(GLint(type), GLsizei(sources.size()), sources.begin());
        if (check(program)) {
            auto shader = Arc<Shader>::alloc(program);

//            std::array<GLenum, 1> properties { GL_NAME_LENGTH };
//            std::array<GLint, 1> values{};

//            GLint count = 0;
//            glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count);
//            for (int i = 0; i < count; ++i) {
//                glGetProgramResourceiv(program, GL_UNIFORM, i, properties.size(), properties.data(), 1, nullptr, values.data());
//
//                auto name = reinterpret_cast<GLchar*>(alloca(values[0]));
//                glGetProgramResourceName(program, GL_UNIFORM, i, values[0], nullptr, name);
//
//                shader->uniforms.emplace(name, Uniform{glGetProgramResourceLocation(program, GL_UNIFORM, name)});
//            }

            return shader;
        }
        glDeleteProgram(program);
        return {};
    }

    auto new_shader(ShaderType type, std::initializer_list<const char *> sources) -> std::shared_ptr<Shader> {
        return Graphics::get()->new_shader(type, sources);
    }
}
