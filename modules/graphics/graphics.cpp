#include "graphics.h"
#include "module.h"
#include "window/window.h"
#include <GL/glew.h>

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

    void rosy::graphics::Graphics::present() noexcept {
        m_window->swapBuffers();
    }

    void Graphics::clear() noexcept {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    ShaderProgram Graphics::newShader(const ShaderType type, const std::initializer_list<const char*> sources) {
        const GLuint program = glCreateShaderProgramv(GLint(type), sources.size(), sources.begin());
        if (validate(program)) {
            ShaderProgram shader{program};

            std::array<GLenum, 1> properties { GL_NAME_LENGTH };
            std::array<GLint, 1> values{};

            {
                GLint count = 0;
                glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count);
                for(int i = 0; i < count; ++i) {
                    glGetProgramResourceiv(program, GL_UNIFORM, i, properties.size(), properties.data(), 1, nullptr, values.data());

                    auto name = reinterpret_cast<GLchar*>(alloca(values[0]));
                    glGetProgramResourceName(program, GL_UNIFORM, i, values[0], nullptr, name);

                    shader.uniforms.emplace(name, Uniform(glGetProgramResourceLocation(program, GL_UNIFORM, name)));
                }
            }

            return std::move(shader);
        }
        glDeleteProgram(program);
        return {0};
    }

    ShaderProgram newShader(const ShaderType type, const std::string_view source) {
        return Graphics::newShader(type, {source.data()});
    }

    ShaderProgram newShader(const ShaderType type, const std::initializer_list<const char*> sources) {
        return Graphics::newShader(type, sources);
    }
}
