#pragma once
#include "physentity.h"

class Evelator : public PhysEntity {
public:
    Evelator() {
        m_direction = Vector(1, 0, 0);
        m_distanceMult = 1.0f;
    }

    virtual void ApplyProperty(std::string key, std::string value) override {
        PhysEntity::ApplyProperty(key, value);

        if (key == "direction") {
            int dir = std::stoi(value);

            switch (dir) {
            case 0:
                m_direction = Vector(1, 0, 0);
                break;
            case 1:
                m_direction = Vector(-1, 0, 0);
                break;
            case 2:
                m_direction = Vector(0, 0, 1);
                break;
            case 3:
                m_direction = Vector(0, 0, -1);
                break;
            }
        } else if (key == "distmult") {
            m_distanceMult = std::stof(value);
        }
    }

    virtual void Spawn() override {
        PhysEntity::Spawn();

        m_shape = PhysicsWorld::ShapeFromModel(m_model, false);
        InitializeRigidbody(m_shape, 0.0f);

        Vector size = m_model->GetMax() - m_model->GetMin();

        m_initialDoorPosition = GetPos();
        m_openDoorPosition = m_initialDoorPosition + m_direction * size * m_distanceMult;
        m_isMoving = false;
        m_isOpen = false;
    }

    virtual void Clean() override {
        PhysEntity::Clean();

        PhysicsWorld::SafeDeleteShape(m_shape);
    }

    virtual void Trigger(bool state) override {
        PhysEntity::Trigger(state);

        if (m_isMoving || state == m_isOpen) {
            return;
        }

        m_isOpen = state;
        m_isMoving = true;
    }

    virtual void FixedUpdate(float delta) override {
        PhysEntity::FixedUpdate(delta);

        if (m_isMoving) {
            Vector target = m_isOpen ? m_openDoorPosition : m_initialDoorPosition;
            Vector direction = (target - GetPos()).normalized();
            Vector movement = direction * m_doorSpeed * m_distanceMult * delta;
            if (movement.length2() > (target - GetPos()).length2()) {
                SetPos(target);
                m_isMoving = false;
            } else {
                SetPos(GetPos() + movement);
            }
        }
    }

private:
    Vector m_direction;
    float m_distanceMult;
    Vector m_initialDoorPosition;
    Vector m_openDoorPosition;
    btCollisionShape* m_shape;
    bool m_isMoving = false;
    bool m_isOpen = false;
    const float m_doorSpeed = 0.5f;
};

ENTCLASS(door_elevator, Evelator)