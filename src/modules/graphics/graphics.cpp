#include "graphics.h"
#include <module.h>
#include <GL/gl3w.h>
#include <modules/window/window.h>

#include <array>

namespace {
    module<rosy::graphics::Graphics> instance;
    module<rosy::window::Window> m_window;

    bool validate(const GLuint program) {
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
            std::cerr << infoLog.data() << std::endl;
            return false;
        }
        return true;
    }
}

namespace rosy::graphics {
    Graphics::Graphics() {}

    Graphics::~Graphics() {}

    void Graphics::present() noexcept {
        m_window->present();
    }

    void Graphics::clear() noexcept {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    auto Graphics::new_shader(ShaderType type, std::initializer_list<const char*> sources) -> std::shared_ptr<Shader> {
        const auto program = glCreateShaderProgramv(GLint(type), sources.size(), sources.begin());
        if (validate(program)) {
            auto shader = std::make_shared<Shader>(Shader{
                .program = program
            });

            std::array<GLenum, 1> properties { GL_NAME_LENGTH };
            std::array<GLint, 1> values{};

            GLint count = 0;
            glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count);
            for (int i = 0; i < count; ++i) {
                glGetProgramResourceiv(program, GL_UNIFORM, i, properties.size(), properties.data(), 1, nullptr, values.data());

                auto name = reinterpret_cast<GLchar*>(alloca(values[0]));
                glGetProgramResourceName(program, GL_UNIFORM, i, values[0], nullptr, name);

                shader->uniforms.emplace(name, Uniform{glGetProgramResourceLocation(program, GL_UNIFORM, name)});
            }

            return std::move(shader);
        }
        glDeleteProgram(program);
        return nullptr;
    }

    auto new_shader(ShaderType type, std::initializer_list<const char *> sources) -> std::shared_ptr<Shader> {
        return instance->new_shader(type, sources);
    }
}
