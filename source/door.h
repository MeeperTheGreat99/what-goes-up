#pragma once
#include "physentity.h"

class Door : public PhysEntity {
public:
    virtual void ApplyProperty(std::string key, std::string value) override;
    virtual void Spawn() override;
    virtual void Clean() override;
    virtual void Use(bool keydown) override;
    virtual void FixedUpdate(float delta) override;

private:
    float m_speed;
    btCollisionShape* m_shape;
    Quaternion m_initialRot;
    Quaternion m_openRot;
    float m_openFraction;
    bool m_moving;
    bool m_open;
};

ENTCLASS(prop_door, Door);