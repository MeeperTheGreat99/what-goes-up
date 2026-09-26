#pragma once
#include "../entity.h"

class Evelator : public Entity {
public:

    Evelator() {
        m_model = Model::LoadExternal("res/models/elevator_door.obj");
    }

    virtual void Spawn() override{
        m_initialDoorPosition = GetPos();
        m_opendoorPosition = m_initialDoorPosition + Vector(1, 0, 0);
        m_isMoving = false;
        m_isOpen = false;
        ToggleDoor();
    }

    virtual void FixedUpdate(float delta) override {
        if (m_isMoving) {
            Vector target = m_isOpen ? m_opendoorPosition : m_initialDoorPosition;
            Vector direction = (target - GetPos()).normalized();
            SetPos(GetPos() + direction * m_doorSpeed * delta);
            if ((target - GetPos()).length() < 0.01f) {
                SetPos(target);
                m_isMoving = false;
            }
        }
    }

    void ToggleDoor() {
        if (m_isMoving) return;
        m_isOpen = !m_isOpen;
        m_isMoving = true;
    }

private:
    bool m_isMoving = false;
    bool m_isOpen = false;
    const float m_doorSpeed = 1.0f;
    Vector m_opendoorPosition;
    Vector m_initialDoorPosition;
};