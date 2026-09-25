#pragma once
#include "model.h"
#include "physics.h"
#include "vector.h"
#include <map>
#include <stdexcept>

typedef uint32_t EID;

class Entity {
public:
    static std::map<EID, Entity*> Entities;

    Entity() {
        EID id = 0;

        while (Entities.count(id)) {
            id++;

            if (id == 0) {
                throw std::runtime_error("entity limit reached");
            }
        }

        m_model = nullptr;
        m_id = id;
        m_pos = 0.0f;
        m_angles = 0.0f;
        Entities[id] = this;
    }

    virtual ~Entity() {
        Entities.erase(m_id);
    }

    EID GetID() const {return m_id;}

    virtual void SetPos(Vector pos) {m_pos = pos;}
    Vector GetPos() const {return m_pos;}

    virtual void SetAngles(Vector angles) {m_angles = angles;}
    Vector GetAngles() const {return m_angles;}

    void Draw() {
        if (m_model) {
            m_model->Draw();
        }
    }

    // Called after the entity is created
    virtual void Spawn() {}
    // Called before the entity is destroyed
    virtual void Clean() {}

    // Called every frame
    virtual void FrameUpdate(float delta) {}
    // Called on a fixed delta after each physics update
    virtual void FixedUpdate(float delta) {}

protected:
    static PhysicsWorld* World;

    Model* m_model;

private:
    EID m_id;
    Vector m_pos;
    Vector m_angles;
};