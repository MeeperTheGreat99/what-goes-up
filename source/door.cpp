#include "door.h"

static const char* models[] = {
    "res/models/door_left.glb",
    "res/models/door_right.glb"
};

void Door::ApplyProperty(std::string key, std::string value) {
    PhysEntity::ApplyProperty(key, value);

    if (key == "model") {
        m_model = Model::LoadExternal(models[std::stoi(value)-1]);
    } else if (key == "speed") {
        m_speed = std::stof(value);
    } else if (key == "direction") {
        m_direction = std::stoi(value);
    }
}

void Door::Spawn() {
    PhysEntity::Spawn();

    m_shape = PhysicsWorld::ShapeFromModel(m_model, false);
    InitializeRigidbody(m_shape, 0.0f);
    m_initialRot = GetRot();
    m_openRot = Quaternion::fromEulerAngles(GetAngles() + Vector(0, m_direction ? -90 : 90, 0));
    m_openFraction = 0.0f;
    m_moving = false;
    m_open = false;
}

void Door::Clean() {
    PhysEntity::Clean();

    PhysicsWorld::SafeDeleteShape(m_shape);
}

void Door::Use(bool keydown) {
    if (keydown && !m_moving) {
        m_moving = true;
        m_open = !m_open;
    }
}

void Door::FixedUpdate(float delta) {
    PhysEntity::FixedUpdate(delta);

    if (!m_moving) {
        return;
    }

    float targetFraction = m_open ? 1.0f : 0.0f;
    float dFraction = targetFraction - m_openFraction;
    bool done = false;
    if (dFraction < 0.0f) {
        m_openFraction -= delta * m_speed;
        if (m_openFraction < 0.0f) {
            done = true;
            m_openFraction = 0.0f;
        }
    } else {
        m_openFraction += delta * m_speed;
        if (m_openFraction > 1.0f) {
            done = true;
            m_openFraction = 1.0f;
        }
    }

    SetRot(Quaternion::Slerp(m_openFraction, m_initialRot, m_openRot));
    if (done) {
        m_moving = false;
    }
}