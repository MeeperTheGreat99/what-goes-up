#pragma once
#include "physentity.h"

class MeshEntity : public PhysEntity {
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

    void SetModelFilename(std::string filename, bool complexCollision) {
        m_model = Model::LoadExternal(filename);
        
    }

    void SetModel(Model* model, bool complexCollision) {
        m_model = model;
    }

private:
    btCollisionShape* m_shape = nullptr;
};