#pragma once
#include "physentity.h"

class MeshEntity : public PhysEntity {
public:
    virtual void Clean() override {
        PhysEntity::Clean();

        PhysicsWorld::SafeDeleteShape(m_shape);
    }

    void SetModelFilename(std::string filename, bool complexCollision) {
        m_model = Model::LoadExternal(filename);
        m_shape = PhysicsWorld::ShapeFromModel(m_model, complexCollision);
    }

private:
    btCollisionShape* m_shape = nullptr;
};