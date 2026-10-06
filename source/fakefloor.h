#pragma once
#include "physentity.h"

class FakeFloor : public PhysEntity {
public:
    FakeFloor();

    virtual void Spawn() override;
    virtual void Clean() override;
    virtual void Trigger(bool state) override;
    virtual void FrameUpdate(float delta) override;
    virtual bool ShouldDraw() override { return !m_triggered; }

private:
    Audio::Source* m_collapseSound;
    btCollisionShape* m_shape;
    bool m_triggered;
};