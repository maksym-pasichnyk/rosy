#pragma once

#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <property.h>
#include <modules/timer/timer.h>

#include "transform.h"

struct Camera {
public:
    enum UpdateState : uint8_t {
        none      = 0u,
        rotate    = 1u,
        move_fb   = 2u,
        move_lr   = 4u,
        updatePos = 6u,
        updateMat = 7u,
    };

    auto camera_to_world_matrix() const noexcept -> glm::mat4 {
        return transform.inverse_world_to_local_matrix();
    }

    void update(rosy::timer::duration dt);
    auto inverse_transform(const glm::vec3& point) -> glm::vec3;

    Transform transform{};
};