#pragma once
#include "physentity.h"

class Button : public PhysEntity {
public:
    virtual void Spawn() override;
    virtual void Clean() override;

private:
    btCollisionShape* m_shape;
};