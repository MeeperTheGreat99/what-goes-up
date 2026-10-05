#pragma once
#include "vector.h"
#include "mapper/plane.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Camera {
public:
    struct Frustum {
        Plane top, bottom;
        Plane right, left;
        Plane near, far;
        Vector renderPos;

        static Frustum FromMatrices(glm::mat4 proj, glm::mat4 view) {
            Frustum ret;
            
            glm::mat4 pv = proj * view;
            ret.left = pv[3] + pv[0];
            ret.right = pv[3] - pv[0];
            ret.bottom = pv[3] + pv[1];
            ret.top = pv[3] - pv[1];
            ret.near = pv[3] + pv[2];
            ret.far = pv[3] - pv[2];

            return ret;
        }

        static Frustum FromCamera(Camera camera) {
            Frustum ret;

            float halfVSide = camera.GetFar() * tanf(glm::radians(camera.GetFOV()) * 0.5f);
            float halfHSize = halfVSide * camera.GetAspect();
            glm::vec3 camFront = camera.GetAng().direction().gl();
            glm::vec3 camRight = glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 camUp = glm::normalize(glm::cross(camRight, camFront));
            glm::vec3 frontMultFar = camera.GetFar() * camFront;

            ret.near = Plane(camera.GetPos() + camera.GetNear() * camFront, camFront);
            ret.far = Plane(camera.GetPos() + frontMultFar, -camFront);
            ret.right = Plane(camera.GetPos(), glm::cross(camUp, frontMultFar + camRight * halfHSize));
            ret.left = Plane(camera.GetPos(), glm::cross(frontMultFar - camRight * halfHSize, camUp));
            ret.top = Plane(camera.GetPos(), glm::cross(camRight, frontMultFar - camUp * halfVSide));
            ret.bottom = Plane(camera.GetPos(), glm::cross(frontMultFar + camUp * halfVSide, camRight));

            return ret;
        }

        bool ClassifySphere(Vector p, float radius) {
            p += renderPos;
            
            if (top.GetDistance(p) < -radius) {
                return false;
            }

            if (bottom.GetDistance(p) < -radius) {
                return false;
            }

            if (left.GetDistance(p) < -radius) {
                return false;
            }

            if (right.GetDistance(p) < -radius) {
                return false;
            }

            if (near.GetDistance(p) < -radius) {
                return false;
            }

            if (far.GetDistance(p) < -radius) {
                return false;
            }

            return true;
        }
    };

    Camera() : m_aspect(1.0f), m_fov(90), m_near(0.1f), m_far(100.0f), m_pDirty(true), m_vDirty(true) {}

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

    void SetNear(float near) {
        m_near = near;
        m_pDirty = true;
    }
    const float GetNear() {return m_near;}

    void SetFar(float far) {
        m_far = far;
        m_pDirty = true;
    }
    const float GetFar() {return m_far;}

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
            m_proj = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
            m_pDirty = false;
        }

        return m_proj;
    }

    glm::mat4 GetView() {
        if (m_vDirty) {
            Vector worldUp(0.0f, 1.0f, 0.0f);
            Vector forward = m_ang.direction();
            Vector right = forward.cross(worldUp).normalized();
            Vector up = right.cross(forward);

            up = glm::rotate(up.gl(), glm::radians(m_ang.z), forward.gl());
            up.normalize();

            m_view = glm::lookAt(m_pos.gl(), (m_pos + forward).gl(), up.gl());
            m_vDirty = false;
        }

        return m_view;
    }

private:
    float m_aspect, m_fov;
    float m_near, m_far;
    Vector m_pos, m_ang;
    bool m_pDirty, m_vDirty;
    glm::mat4 m_proj;
    glm::mat4 m_view;
};