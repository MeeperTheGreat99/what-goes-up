#pragma once
#include "entity.h"

class Impulse : public Entity {
public:
    virtual void Spawn() override;
    virtual void Clean() override;
    virtual void Trigger(bool state) override;
    virtual void FrameUpdate(float delta) override;

private:
    Audio::Source* m_source;
    Audio::Sequence* m_collapseAudio;
    float m_titleCardTime;
    float m_mapChangeTime;
};