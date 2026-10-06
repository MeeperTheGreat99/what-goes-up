#pragma once
#include "entity.h"

class Trigger : public Entity {
public:
    virtual bool ShouldDraw() override { return false; }
    virtual void Spawn() override;
    virtual void FixedUpdate(float delta) override;

private:
    Vector m_min, m_max;
    bool m_triggered;
};