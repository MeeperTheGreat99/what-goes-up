#pragma once
#include "physentity.h"

class Evelator : public PhysEntity {
public:
    Evelator() {
        m_model = Model::LoadExternal("res/models/elevator.fbx");
    }

    virtual void Spawn() override {
        PhysEntity::Spawn();

        m_shape = PhysicsWorld::ShapeFromModel(m_model, false);
        InitializeRigidbody(m_shape, 0.0f);

        m_initialDoorPosition = GetPos();
        m_opendoorPosition = m_initialDoorPosition + Vector(1, 0, 0);
        m_isMoving = false;
        m_isOpen = false;
        ToggleDoor();
    }

    virtual void Clean() override {
        PhysEntity::Clean();

        PhysicsWorld::SafeDeleteShape(m_shape);
    }

    virtual void Use(bool keydown) override {
        if (keydown) {
            ToggleDoor();
        }
    }

    virtual void FixedUpdate(float delta) override {
        PhysEntity::FixedUpdate(delta);

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
    btCollisionShape* m_shape;
    bool m_isMoving = false;
    bool m_isOpen = false;
    const float m_doorSpeed = 1.0f;
    Vector m_opendoorPosition;
    Vector m_initialDoorPosition;
};