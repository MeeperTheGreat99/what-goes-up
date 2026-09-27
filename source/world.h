#pragma once
#include "physentity.h"

class World : public PhysEntity {
public:
    World() {
        m_model = Model::LoadExternal("res/models/floor.obj");
    }

    virtual void Spawn() override {
        PhysEntity::Spawn();

        m_shape = PhysicsWorld::ShapeFromModel(m_model, true);
        InitializeRigidbody(m_shape, 0.0f);
    }

    virtual void Clean() override {
        PhysEntity::Clean();

        PhysicsWorld::SafeDeleteShape(m_shape);
    }

private:
    btCollisionShape* m_shape;
};