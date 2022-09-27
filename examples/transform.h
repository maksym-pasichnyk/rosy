#pragma once

#include <iostream>
#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <property.hpp>

struct Transform {
public:
    static auto TRS(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) -> glm::mat4 {
        return glm::scale(glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation), scale);
    }
    
    void set_position(const glm::vec3 &v) {
        if (m_worldPosition != v) {
            m_dirty = true;
            m_worldPosition = v;
        }
    }
    
    void set_rotation(const glm::quat &v) {
        if (m_worldRotation != v) {
            m_dirty = true;
            m_worldRotation = v;
        }
    }

    auto get_position() const -> glm::vec3 {
        return m_worldPosition;
    }
    
    auto get_rotation() const -> glm::quat {
        return m_worldRotation;
    }

    property<glm::vec3, property<>::get> forward = [&] {
        if (m_dirty) updateMatrices();
        return m_forward;
    };

    property<glm::vec3, property<>::get> backward = [&] {
        if (m_dirty) updateMatrices();
        return m_backward;
    };

    property<glm::vec3, property<>::get> up = [&] {
        if (m_dirty) updateMatrices();
        return m_up;
    };

    property<glm::vec3, property<>::get> down = [&] {
        if (m_dirty) updateMatrices();
        return m_down;
    };

    property<glm::vec3, property<>::get> left = [&] {
        if (m_dirty) updateMatrices();
        return m_left;
    };

    property<glm::vec3, property<>::get> right = [&] {
        if (m_dirty) updateMatrices();
        return m_right;
    };

    auto local_to_world_matrix() const -> glm::mat4 {
        if (m_dirty) updateMatrices();
        return m_localToWorldMatrix;
    }

    auto world_to_local_matrix() const -> glm::mat4 {
        if (m_dirty) updateMatrices();
        return m_worldToLocalMatrix;
    }

    auto inverse_world_to_local_matrix() const -> glm::mat4 {
        if (m_dirty) updateMatrices();
        return m_inverseWorldToLocalMatrix;
    }

private:
    mutable glm::vec3 m_worldPosition = {0, 0, 0};
    mutable glm::quat m_worldRotation = {1, 0, 0, 0};
//    mutable glm::vec3 m_worldScale = {1, 1, 1};

    mutable glm::vec3 m_forward, m_backward;
    mutable glm::vec3 m_up, m_down;
    mutable glm::vec3 m_left, m_right;

    mutable glm::mat4 m_localToWorldMatrix;
    mutable glm::mat4 m_worldToLocalMatrix;
    mutable glm::mat4 m_inverseWorldToLocalMatrix;

    mutable bool m_dirty = true;

    void updateMatrices() const {
        m_dirty = false;

        m_localToWorldMatrix = TRS(get_position(), get_rotation(), {1, 1, 1});
        m_worldToLocalMatrix = TRS(get_position(), glm::conjugate(get_rotation()), {1, 1, -1});
        m_inverseWorldToLocalMatrix = glm::inverse(m_worldToLocalMatrix);

        m_backward = -(m_forward = (get_rotation() * glm::vec3{0, 0, -1}));
        m_down = -(m_up = get_rotation() * glm::vec3{0, 1, 0});
        m_left = -(m_right = get_rotation() * glm::vec3{1, 0, 0});
    }
};