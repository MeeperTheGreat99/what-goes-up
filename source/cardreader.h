#pragma once
#include "physentity.h"

class CardReader : public PhysEntity {
public:
    CardReader();

    virtual void Spawn() override;
    virtual void Clean() override;
    virtual void Use(bool keydown) override;
    virtual void FrameUpdate(float delta) override;

private:
    btCollisionShape* m_shape;
    Audio::Source* m_accessSound;
    int m_swipes;
    bool m_pranked;
};