#pragma once
#include "physentity.h"

class Elevator : public PhysEntity {
public:
    Elevator();

    virtual void ApplyProperty(std::string key, std::string value) override;
    virtual void Spawn() override;
    virtual void Clean() override;
    virtual void Trigger(bool state) override;
    virtual void FixedUpdate(float delta) override;

private:
    Vector m_travel;
    float m_speed;
    std::string m_firstDoor;
    std::string m_secondDoor;
    btCollisionShape* m_shape;
    Vector m_firstPosition;
    Vector m_secondPosition;
    bool m_isMoving;

    void MoveDoor(std::string identifier, bool open);
};