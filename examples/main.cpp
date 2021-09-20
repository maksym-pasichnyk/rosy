#include <rosy.h>
#include <modules/audio/audio.h>
#include <modules/window/window.h>
#include <modules/graphics/shader.h>
#include <modules/graphics/graphics.h>

#include <array>
#include <fstream>
#include <filesystem>

#include "camera.h"

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

    std::shared_ptr<Shader> vertex;
    std::shared_ptr<Shader> fragment;
    rosy::timer::point start_time;

    GLuint pipeline, vbo, ibo, vao;

    Camera camera{};
    VertexData* buffer;

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

    auto core_glsl = read_file("../examples/core.glsl").value();
    auto main_glsl = read_file("../examples/fractal.glsl").value();

    vertex = rosy::graphics::new_shader(ShaderType::Vertex, {
        R"(
            #version 400
            #define VERTEX_SHADER
        )",
        core_glsl.c_str(),
        "\n",
        main_glsl.c_str()
    });

    fragment = rosy::graphics::new_shader(ShaderType::Fragment, {
        R"(
            #version 400
            #define FRAGMENT_SHADER
        )",
        core_glsl.c_str(),
        "\n",
        main_glsl.c_str()
    });

    glCreateProgramPipelines(1, &pipeline);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertex->program);
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragment->program);
    glBindProgramPipeline(pipeline);

    const auto position = vertex->get_attrib_location("in_data.position").value();
    const auto uv = vertex->get_attrib_location("in_data.uv").value();

    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(VertexData) * 4, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    glCreateBuffers(1, &ibo);
    glNamedBufferData(ibo, sizeof(indices), indices.data(), GL_STATIC_DRAW);

    glCreateVertexArrays(1, &vao);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(VertexData));
    glVertexArrayElementBuffer(vao, ibo);

    glEnableVertexArrayAttrib(vao, position.id);
    glEnableVertexArrayAttrib(vao, uv.id);

    glVertexArrayAttribFormat(vao, position.id, 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, x));
    glVertexArrayAttribFormat(vao, uv.id, 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, u));

    glVertexArrayAttribBinding(vao, position.id, 0);
    glVertexArrayAttribBinding(vao, uv.id, 0);

    buffer = reinterpret_cast<VertexData*>(glMapNamedBufferRange(vbo, 0, sizeof(VertexData) * 4, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));

    std::memcpy(buffer, vertices.data(), sizeof(vertices));

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
    glUnmapNamedBuffer(vbo);
}

void rosy::draw() {
    glBindVertexArray(vao);
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

    buffer[0].u = -aspect * HALF_FOV_60_TAN + y_scale;
    buffer[0].v = -HALF_FOV_60_TAN + y_scale;

    buffer[1].u = aspect * HALF_FOV_60_TAN + y_scale;
    buffer[1].v = -HALF_FOV_60_TAN + y_scale;

    buffer[2].u = aspect * HALF_FOV_60_TAN + y_scale;
    buffer[2].v = HALF_FOV_60_TAN + y_scale;

    buffer[3].u = -aspect * HALF_FOV_60_TAN + y_scale;
    buffer[3].v = HALF_FOV_60_TAN + y_scale;

    fragment->set_int2("_Resolution", glm::ivec2{width, height});
    vertex->set_int2("_Resolution", glm::ivec2{width, height});
}
