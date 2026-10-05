#pragma once
#include "physentity.h"

class ModelProp : public PhysEntity {
public:
    virtual void ApplyProperty(std::string key, std::string value) override;
    virtual void Spawn() override;
    virtual void Clean() override;

private:
    btCollisionShape* m_shape;
};