#pragma once
#include "vector.h"

class Light {
public:
    Light();
    ~Light();

    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }

    void SetPos(const Vector& pos) { m_pos = pos; }
    Vector GetPos() const { return m_pos; }

    void SetColor(const Vector& color) { m_color = color.normalized(); }
    Vector GetColor() const { return m_color; }

    void SetIntensity(float intensity) { m_intensity = intensity; }
    float GetIntensity() const { return m_intensity; }

    float GetRadius() const { return sqrtf(m_intensity * 256.0f); }

private:
    bool m_enabled = true;
    Vector m_pos = 0.0f;
    Vector m_color = 1.0f;
    float m_intensity = 1.0f;
};