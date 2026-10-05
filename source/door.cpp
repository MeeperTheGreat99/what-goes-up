#include "door.h"

static const char* models[] = {
    "res/models/door_left.glb",
    "res/models/door_right.glb"
};

Door::Door() {
    m_openSound = Audio::Instance->LoadSample("res/sounds/environment/door_open.wav");
    m_closeSound = Audio::Instance->LoadSample("res/sounds/environment/door_close.wav");
    m_lockedSound = Audio::Instance->LoadSample("res/sounds/environment/door_locked.wav");
}

void Door::ApplyProperty(std::string key, std::string value) {
    PhysEntity::ApplyProperty(key, value);

    if (key == "model") {
        m_modelFlip = (std::stoi(value)-1) % 2;
        m_model = Model::LoadExternal(models[std::stoi(value)-1]);
    } else if (key == "speed") {
        m_speed = std::stof(value);
    } else if (key == "direction") {
        m_direction = std::stoi(value);
    }
}

void Door::Spawn() {
    PhysEntity::Spawn();

    glm::quat delta = Quaternion::fromEulerAngles(Vector(0, m_direction ? 270 : 90, 0)).gl();
    m_audioSource = new Audio::Source(nullptr);
    m_audioSource->Set3D();
    m_squeakSource = new Audio::Source(Audio::Instance->LoadSample("res/sounds/environment/door_squeak.wav"));
    m_squeakSource->Set3D();
    m_locked = m_spawnflags & 1;
    m_shape = PhysicsWorld::ShapeFromModel(m_model, false);
    m_initialRot = GetRot();
    m_openRot = m_initialRot.gl() * delta;
    m_openFraction = 0.0f;
    m_moving = false;
    m_open = false;

    InitializeRigidbody(m_shape, 0.0f);
}

void Door::Clean() {
    PhysEntity::Clean();

    delete m_audioSource;
    PhysicsWorld::SafeDeleteShape(m_shape);
}

void Door::Use(bool keydown) {
    if (keydown && !m_moving) {
        if (!m_open && m_locked) {
            m_audioSource->SetSample(m_lockedSound);
            m_audioSource->Play();
            return;
        }

        m_moving = true;
        m_open = !m_open;

        if (m_open) {
            m_audioSource->SetSample(m_openSound);
            m_audioSource->Play();
        }

        if (rand() % 3 == 0) {
            m_squeakSource->Play();
        }
    }
}

void Door::FixedUpdate(float delta) {
    PhysEntity::FixedUpdate(delta);

    if (!m_moving) {
        return;
    }

    m_audioSource->SetPos(GetPos());
    m_squeakSource->SetPos(GetPos());

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
        
        if (!m_open) {
            m_audioSource->SetSample(m_closeSound);
            m_audioSource->Play();
        }
    }
}