#pragma once
#include "entity.h"
#include "audio.h"

class ReverbZone : public Entity {
public:
    virtual bool ShouldDraw() override { return false; }
    virtual void ApplyProperty(std::string key, std::string value) override;
    virtual void Spawn() override;
    virtual void FixedUpdate(float delta) override;

    EFXEAXREVERBPROPERTIES* GetProperties() { return &m_reverb; }

private:
    EFXEAXREVERBPROPERTIES m_reverb;
    Vector m_min, m_max;
};