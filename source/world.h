#pragma once
#include "physentity.h"

class World : public PhysEntity {
public:
    virtual void Spawn() override {
        PhysEntity::Spawn();

        InitializeRigidbody(m_shape, 0.0f);
    }

    virtual void Clean() override {
        PhysEntity::Clean();

        PhysicsWorld::SafeDeleteShape(m_shape);
    }

    void AssumeShape(btCollisionShape* shape) {
        m_shape = shape;
    }

private:
    btCollisionShape* m_shape = nullptr;
};

ENTCLASS(worldspawn, World)