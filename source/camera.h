#pragma once
#include "vector.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera() : m_aspect(1.0f), m_fov(90), m_pDirty(true), m_vDirty(true) {}

    void SetAspect(float aspect) {
        m_aspect = aspect;
        m_pDirty = aspect;
    }
    const float GetAspect() const {return m_aspect;}

    void SetFOV(float fov) {
        m_fov = fov;
        m_pDirty = true;
    }
    const float GetFOV() {return m_fov;}

    void SetPos(Vector pos) {
        m_pos = pos;
        m_vDirty = true;
    }
    const Vector GetPos() const {return m_pos;}

    void SetAng(Vector ang) {
        m_ang = ang;
        m_vDirty = true;
    }
    const Vector GetAng() const {return m_ang;}

    glm::mat4 GetProj() {
        if (m_pDirty) {
            m_proj = glm::perspective(glm::radians(m_fov), m_aspect, 0.1f, 200.0f);
            m_pDirty = false;
        }

        return m_proj;
    }

    glm::mat4 GetView() {
        if (m_vDirty) {
            Vector forward = m_ang.direction();
            Vector right = forward.cross(Vector(0.0f, 1.0f, 0.0f));
            Vector up = right.cross(forward);
            m_view = glm::lookAt(m_pos.gl(), (m_pos + forward).gl(), up.gl());
            // m_view *= glm::rotate(glm::mat4(1.0f), glm::radians(m_ang.z), forward.gl());
            m_vDirty = false;
        }

        return m_view;
    }

private:
    float m_aspect, m_fov;
    Vector m_pos, m_ang;
    bool m_pDirty, m_vDirty;
    glm::mat4 m_proj;
    glm::mat4 m_view;
};