#include "modelprop.h"

void ModelProp::ApplyProperty(std::string key, std::string value) {
    PhysEntity::ApplyProperty(key, value);

    if (key == "mesh") {
        m_model = Model::LoadExternal(value);
    }
}

void ModelProp::Spawn() {
    PhysEntity::Spawn();

    bool dynamic = m_spawnflags & 1;
    m_shape = PhysicsWorld::ShapeFromModel(m_model, !dynamic);
    InitializeRigidbody(m_shape, dynamic ? 10.0f : 0.0f);
}

void ModelProp::Clean() {
    PhysEntity::Clean();

    PhysicsWorld::SafeDeleteShape(m_shape);
}

ENTCLASS(prop_physics_model, ModelProp)