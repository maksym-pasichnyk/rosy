#include <rosy.hpp>
#include "modules/audio/audio.hpp"
#include "modules/window/window.hpp"
#include <modules/graphics/shader.hpp>
#include "modules/graphics/graphics.hpp"

#include <array>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "camera.h"
#include "fmt/core.h"

struct VertexAttrib {
    uint32_t binding;
    int32_t size;
    GLenum type;
    bool normalized;
    uint64_t offset;
};

struct Mesh {
    static auto create() {
        auto out = std::make_shared<Mesh>();

        glGenVertexArrays(1, &out->vao);
        glGenBuffers(1, &out->vbo);
        glGenBuffers(1, &out->ibo);

        glBindVertexArray(out->vao);
        glBindBuffer(GL_ARRAY_BUFFER, out->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out->ibo);
        glBindVertexArray(0);

        return out;
    }

    void set_layout(std::span<const VertexAttrib> attribs, uint64_t stride) {
        vertex_stride = stride;

        glBindVertexArray(vao);
        for (auto& attrib : attribs) {
            glEnableVertexAttribArray(attrib.binding);
            glVertexAttribPointer(attrib.binding, attrib.size, attrib.type, attrib.normalized, int32_t(stride), (void*) attrib.offset);
        }
        glBindVertexArray(0);
    }

    void set_indices(const void* data, size_t count, size_t stride) {
        index_count = count;
        index_stride = stride;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, uint32_t(index_count * index_stride), data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    }

    void set_vertices(const void* data, size_t count) {
        vertex_count = count;

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, uint32_t(vertex_count * vertex_stride), data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint get_native_handle() {
        return vao;
    }

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;

    size_t vertex_stride = 0;
    size_t vertex_count = 0;

    size_t index_count = 0;
    size_t index_stride = 0;
};

namespace {
    constexpr auto HALF_FOV_60_TAN = 0.577350269f;

    using rosy::graphics::ShaderType;
    using rosy::graphics::Shader;
    using rosy::graphics::Attrib;
    using rosy::audio::Source;
    using rosy::audio::SourceType;

    struct VertexData {
        float x, y, u, v;
    };

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> vertex;
    std::shared_ptr<Shader> fragment;
    rosy::timer::point start_time;

    GLuint pipeline;

    Camera camera{};
//    VertexData* buffer;

    constexpr auto indices = std::array { 0, 1, 3, 1, 2, 3 };
    constexpr auto vertices = std::array{
        VertexData{-1.0f, -1.0f, 0, 0},
        VertexData{ 1.0f, -1.0f, 1, 0},
        VertexData{ 1.0f,  1.0f, 1, 1},
        VertexData{-1.0f,  1.0f, 0, 1}
    };

    std::shared_ptr<Source> sound;
}

static auto read_file(const std::filesystem::path& path) -> std::optional<std::string> {
    if (std::ifstream file{path, std::ios::in}) {
        std::stringstream stream{};
        stream << file.rdbuf();
        return stream.str();
    }
    return std::nullopt;
}

void rosy::conf(Config& c) {
    c.window = {
        .title = "RayMarching",
        .width = 800,
        .height = 600
    };
}

void rosy::load() {
    sound = rosy::audio::new_source("../examples/supernatural.wav", SourceType::Stream);
    sound->set_loop(true);
    sound->play();

    auto parse = [](const std::string& filename) {
        std::string input = read_file(filename).value();

        while (true) {
            size_t include_char_index = input.find("#include", 0);
            if (include_char_index == std::string::npos) {
                return input;
            }

            size_t first_quote_index = input.find('"', include_char_index);
            if (first_quote_index == std::string::npos) {
                fmt::print("Expected filename\n");
                exit(1);
            }
            size_t second_quote_index = input.find('"', first_quote_index + 1);
            if (second_quote_index == std::string::npos) {
                fmt::print("Expected filename\n");
                exit(1);
            }

            std::string file_name = input.substr(first_quote_index + 1, second_quote_index - first_quote_index - 1);
            fmt::print("include '{}'\n", file_name);

            std::string tmp;
            tmp.append(input.substr(0, include_char_index));
            tmp.append(read_file(file_name).value());
            tmp.append(input.substr(second_quote_index + 1));

            std::swap(input, tmp);
        }
    };

    auto main_glsl = parse("../examples/fractal.glsl");

    vertex = rosy::graphics::new_shader(ShaderType::Vertex, {
        "#version 400\n",
        "#define VERTEX_SHADER\n",
        main_glsl.c_str()
    });

    fragment = rosy::graphics::new_shader(ShaderType::Fragment, {
        "#version 400\n",
        "#define FRAGMENT_SHADER\n",
        main_glsl.c_str()
    });

    glGenProgramPipelines(1, &pipeline);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertex->program);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragment->program);
    glBindProgramPipeline(pipeline);

    const auto position = vertex->get_attrib_location("in_position").value();
    const auto uv = vertex->get_attrib_location("in_uv").value();

    mesh = Mesh::create();

    mesh->set_layout(std::array{
        VertexAttrib{uint32_t(position.id), 2, GL_FLOAT, false, offsetof(VertexData, x)},
        VertexAttrib{uint32_t(uv.id),       2, GL_FLOAT, false, offsetof(VertexData, u)}
    }, sizeof(VertexData));

    mesh->set_vertices(vertices.data(), vertices.size());
    mesh->set_indices(indices.data(), indices.size(), sizeof(uint32_t));

    camera.transform.set_position(glm::vec3{0, 0, 3});

    fragment->set_float3("_WorldSpaceCameraPos", camera.transform.get_position());
    fragment->set_float4x4("_LocalToWorldMatrix", camera.transform.local_to_world_matrix());
    fragment->set_float4x4("_CameraToWorldMatrix", camera.camera_to_world_matrix());

    vertex->set_float3("_WorldSpaceCameraPos", camera.transform.get_position());
    vertex->set_float4x4("_LocalToWorldMatrix", camera.transform.local_to_world_matrix());
    vertex->set_float4x4("_CameraToWorldMatrix", camera.camera_to_world_matrix());

    resize(rosy::window::get_width(), rosy::window::get_height());

    rosy::window::set_relative_mouse(true);

    start_time = rosy::timer::get_time();
}

void rosy::unload() {
}

void rosy::draw() {
    glBindVertexArray(mesh->get_native_handle());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void rosy::focus(bool status) {

}

void rosy::update(timer::duration dt) {
    camera.update(dt);

    const auto position = camera.transform.get_position();
    const auto camera_to_world_matrix = camera.camera_to_world_matrix();
    const auto local_to_world_matrix = camera.transform.local_to_world_matrix();

    vertex->set_float3("_WorldSpaceCameraPos", position);
    fragment->set_float3("_WorldSpaceCameraPos", position);

    vertex->set_float4x4("_LocalToWorldMatrix", local_to_world_matrix);
    fragment->set_float4x4("_LocalToWorldMatrix", local_to_world_matrix);

    vertex->set_float4x4("_CameraToWorldMatrix", camera_to_world_matrix);
    fragment->set_float4x4("_CameraToWorldMatrix", camera_to_world_matrix);

    const auto time_since_load = static_cast<float>(std::chrono::duration<double>(rosy::timer::get_time() - start_time).count());
    fragment->set_float("_Time", time_since_load);
    vertex->set_float("_Time", time_since_load);
}

void rosy::resize(int width, int height) {
    glViewport(0, 0, width, height);

    const auto y_scale = 1.0f / static_cast<float>(height);
    const auto aspect = static_cast<float>(width) * y_scale;

    auto buffer = std::array{
        VertexData{-1.0f, -1.0f, 0, 0},
        VertexData{ 1.0f, -1.0f, 1, 0},
        VertexData{ 1.0f,  1.0f, 1, 1},
        VertexData{-1.0f,  1.0f, 0, 1}
    };

    buffer[0].u = -aspect * HALF_FOV_60_TAN + y_scale;
    buffer[0].v = -HALF_FOV_60_TAN + y_scale;

    buffer[1].u = aspect * HALF_FOV_60_TAN + y_scale;
    buffer[1].v = -HALF_FOV_60_TAN + y_scale;

    buffer[2].u = aspect * HALF_FOV_60_TAN + y_scale;
    buffer[2].v = HALF_FOV_60_TAN + y_scale;

    buffer[3].u = -aspect * HALF_FOV_60_TAN + y_scale;
    buffer[3].v = HALF_FOV_60_TAN + y_scale;

    mesh->set_vertices(buffer.data(), buffer.size());

    fragment->set_int2("_Resolution", glm::ivec2{width, height});
    vertex->set_int2("_Resolution", glm::ivec2{width, height});
}
