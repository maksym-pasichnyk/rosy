#pragma once

#include <map>
#include <string>
#include <optional>

#include <GL/gl3w.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace rosy::graphics {
    struct Uniform { GLint id; };
    struct Attrib { GLint id; };

    enum class ShaderType {
        Fragment = GL_FRAGMENT_SHADER,
        Vertex   = GL_VERTEX_SHADER,
        Compute  = GL_COMPUTE_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
    };

    struct Shader {
        GLuint program;

        std::map<std::string, Uniform> uniforms{};
        std::map<std::string, Attrib> attributes{};

        auto get_uniform_location(const std::string& name) noexcept -> std::optional<Uniform> {
            if (auto it = uniforms.find(name); it != uniforms.end()) {
                return it->second;
            }
            return std::nullopt;
        }

        auto get_attrib_location(const std::string& name) noexcept -> std::optional<Attrib> {
            const auto id = glGetAttribLocation(program, name.c_str());
            if (id != -1) {
                return Attrib{id};
            }
            return std::nullopt;
        }

        void use_program_stages(GLbitfield stages, const Shader& shader) noexcept {
            glUseProgramStages(program, stages, shader.program);
        }

        void set_int(Uniform location, GLint p) noexcept {
            glProgramUniform1i(program, location.id, p);
        }

        void set_int2(Uniform location, const glm::ivec2& p) noexcept {
            glProgramUniform2i(program, location.id, p.x, p.y);
        }

        void set_int3(Uniform location, const glm::ivec3& p) noexcept {
            glProgramUniform3i(program, location.id, p.x, p.y, p.z);
        }

        void set_float(Uniform location, GLfloat p) noexcept {
            glProgramUniform1f(program, location.id, p);
        }

        void set_float2(Uniform location, const glm::vec2& p) noexcept {
            glProgramUniform2f(program, location.id, p.x, p.y);
        }

        void set_float3(Uniform location, const glm::vec3& p) noexcept {
            glProgramUniform3f(program, location.id, p.x, p.y, p.z);
        }

        void set_float4x4(Uniform location, const glm::mat4& m) noexcept {
            glProgramUniformMatrix4fv(program, location.id, 1, false, glm::value_ptr(m));
        }

        void set_int(const std::string& name, GLint p) noexcept {
            if (auto id = get_uniform_location(name)) {
                set_int(*id, p);
            }
        }

        void set_int2(const std::string& name, const glm::ivec2& p) noexcept {
            if (auto id = get_uniform_location(name)) {
                set_int2(*id, p);
            }
        }

        void set_int3(const std::string& name, const glm::ivec3& p) noexcept {
            if (auto id = get_uniform_location(name)) {
                set_int3(*id, p);
            }
        }

        void set_float(const std::string& name, GLfloat p) noexcept {
            if (auto id = get_uniform_location(name)) {
                set_float(*id, p);
            }
        }

        void set_float2(const std::string& name, const glm::vec2& p) noexcept {
            if (auto id = get_uniform_location(name)) {
                set_float2(*id, p);
            }
        }

        void set_float3(const std::string& name, const glm::vec3& p) noexcept {
            if (auto id = get_uniform_location(name)) {
                set_float3(*id, p);
            }
        }

        void set_float4x4(const std::string& name, const glm::mat4& m) noexcept {
            if (auto id = get_uniform_location(name)) {
                set_float4x4(*id, m);
            }
        }
    };
}