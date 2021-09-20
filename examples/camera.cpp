#include "camera.h"

#include <modules/input/input.h>
#include <modules/graphics/graphics.h>

namespace {
    module<rosy::input::Input> m_input;
    module<rosy::graphics::Graphics> m_graphics;
}

auto Camera::inverse_transform(const glm::vec3& point) -> glm::vec3 {
    return glm::conjugate(transform.get_rotation()) * (
        glm::vec4(point, 1.0f) * transform.local_to_world_matrix()
    );
}

void Camera::update(rosy::timer::duration dt) {
    const auto sec = static_cast<float>(std::chrono::duration<double>(dt).count());

    if (m_input->get_mouse_moved()) {
        const auto mouse_delta = m_input->get_mouse_delta();
        const auto delta = -glm::normalize(glm::vec2(mouse_delta)) * sec * 2.0f;
        const auto q_up = glm::angleAxis(delta.y, glm::vec3(1,0,0));
        const auto q_right = glm::angleAxis(delta.x, glm::vec3(0,1,0));

        transform.set_rotation(glm::normalize(q_up * (transform.get_rotation() * q_right)));
    }

    if (m_input->get_key(SDL_SCANCODE_W)) {
        transform.set_position(transform.get_position() + inverse_transform(transform.forward.m_get()) * sec);
    }
    if (m_input->get_key(SDL_SCANCODE_S)) {
        transform.set_position(transform.get_position() + inverse_transform(transform.backward.m_get()) * sec);
    }
    if (m_input->get_key(SDL_SCANCODE_A)) {
        transform.set_position(transform.get_position() + inverse_transform(transform.left.m_get()) * sec);
    }
    if (m_input->get_key(SDL_SCANCODE_D)) {
        transform.set_position(transform.get_position() + inverse_transform(transform.right.m_get()) * sec);
    }
}