#pragma once
#include "physentity.h"

class Door : public PhysEntity {
public:
    Door();

    virtual void ApplyProperty(std::string key, std::string value) override;
    virtual void Spawn() override;
    virtual void Clean() override;
    virtual void Use(bool keydown) override;
    virtual void FixedUpdate(float delta) override;

private:
    Audio::Sample* m_openSound;
    Audio::Sample* m_closeSound;
    Audio::Sample* m_squeakSound;
    Audio::Source* m_audioSource;
    float m_speed;
    bool m_modelFlip;
    bool m_direction = false;
    btCollisionShape* m_shape;
    Quaternion m_initialRot;
    Quaternion m_openRot;
    float m_openFraction;
    bool m_moving;
    bool m_open;
};

ENTCLASS(prop_door, Door);