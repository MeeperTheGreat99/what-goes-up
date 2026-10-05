#include "elevator.h"

Elevator::Elevator() {
    m_speed = 1.0f;
}

void Elevator::ApplyProperty(std::string key, std::string value) {
    PhysEntity::ApplyProperty(key, value);

    if (key == "travel") {
        m_travel = Vector::fromOrigin(value);
    } else if (key == "speed") {
        m_speed = std::stof(value);
    } else if (key == "firstdoor") {
        m_firstDoor = value;
    } else if (key == "seconddoor") {
        m_secondDoor = value;
    }
}

void Elevator::Spawn() {
    PhysEntity::Spawn();

    m_shape = PhysicsWorld::ShapeFromModel(m_model, true);
    InitializeRigidbody(m_shape, 0.0f);

    m_firstPosition = GetPos();
    m_secondPosition = m_firstPosition + m_travel;
    m_isMoving = false;
}

void Elevator::Clean() {
    PhysEntity::Clean();

    PhysicsWorld::SafeDeleteShape(m_shape);
}

void Elevator::Trigger(bool state) {
    if (m_isMoving) {
        return;
    } else if (m_triggerState == state) {
        MoveDoor(m_triggerState ? m_secondDoor : m_firstDoor, true);
        return;
    }

    MoveDoor(m_triggerState ? m_secondDoor : m_firstDoor, false);
    PhysEntity::Trigger(state);

    m_isMoving = true;
}

void Elevator::FixedUpdate(float delta) {
    if (!m_isMoving) {
        return;
    }

    Vector target = m_triggerState ? m_secondPosition : m_firstPosition;
    Vector movement = (target - GetPos()).normalized() * m_speed * delta;
    
    if ((movement.length2() > (target - GetPos()).length2())) {
        SetPos(target);
        MoveDoor(m_triggerState ? m_secondDoor : m_firstDoor, true);

        m_isMoving = false;
    } else {
        SetPos(GetPos() + movement);
    }
}

void Elevator::MoveDoor(std::string identifier, bool open) {
    std::vector<Entity*> entities = FindTargets(identifier);

    for (Entity* entity : entities) {
        entity->Trigger(open);
    }
}

ENTCLASS(elevator, Elevator)