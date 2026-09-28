#pragma once
#include "physentity.h"

class World : public PhysEntity {
public:
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

ENTCLASS(worldspawn, World)