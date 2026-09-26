#pragma once
#include "physentity.h"

class World : public PhysEntity {
public:
    World() {
        m_model = Model::LoadExternal("res/models/floor.obj");
    }

    virtual void Spawn() override {
        PhysEntity::Spawn();

        InitializeRigidbody(PhysicsWorld::ShapeFromMesh(m_model->GetMeshes()[0], false), 0.0f);
    }
};