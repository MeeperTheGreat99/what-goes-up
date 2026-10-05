#include "button.h"

void Button::Spawn() {
    PhysEntity::Spawn();

    m_shape = PhysicsWorld::ShapeFromModel(m_model, true);
    InitializeRigidbody(m_shape, 0.0f);
}

void Button::Clean() {
    PhysEntity::Clean();

    PhysicsWorld::SafeDeleteShape(m_shape);
}

ENTCLASS(button, Button)