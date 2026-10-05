#pragma once
#include "model.h"
#include "physics.h"
#include "vector.h"
#include <map>
#include <stdexcept>
#include <functional>

typedef uint32_t EID;

class Entity {
public:
    friend class Map;

    static std::map<EID, Entity*> Entities;
    static PhysicsWorld* World;
    static float WorldTime;

    static Entity* Create(std::string classname);
    static std::vector<Entity*> FindTargets(std::string target);

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
        m_rot = Quaternion();
        Entities[id] = this;
    }

    virtual ~Entity() {
        if (m_spawned) {
            Clean();
        }

        if (m_model) {
            delete m_model;
        }
        
        Entities.erase(m_id);
    }

    EID GetID() const {return m_id;}

    virtual void SetPos(Vector pos) {m_pos = pos;}
    virtual Vector GetPos() const {return m_pos;}

    virtual void SetRot(Quaternion rot) {m_rot = rot;}
    virtual Quaternion GetRot() const {return m_rot;}

    virtual void SetAngles(Vector angles) {
        SetRot(Quaternion::fromEulerAngles(angles));
    }
    Vector GetAngles() const {
        return GetRot().toEulerAngles();
    }

    virtual bool ShouldDraw() { return true; }
    void Draw(ObjectShader* shader, Camera::Frustum* frustum = nullptr) {
        if (m_model) {
            m_model->Draw(shader, frustum);
        }
    }

    float GetModelRadius() {
        return m_model ? m_model->GetRadius() : 0.0f;
    }

    virtual void ApplyProperty(std::string key, std::string value) {
        if (key == "origin") {
            SetPos(Vector::fromOrigin(value));
        } else if (key == "angles") {
            SetAngles(Vector::fromAngles(value));
        } else if (key == "identifier") {
            m_identifier = value;
        } else if (key == "target") {
            m_target = value;
        } else if (key == "triggertype") {
            m_triggerType = std::stoi(value);
        }
    }

    // Called after the entity is created
    virtual void Spawn() {
        m_spawned = true;
        m_triggerState = false;
    }
    // Called before the entity is destroyed
    virtual void Clean() {m_spawned = false;}

    virtual void Trigger(bool state) { m_triggerState = state; }
    bool IsTriggered() const { return m_triggerState; }
    virtual bool IsTriggerable() const { return true; }

    // Called when the player presses/releases their use key while looking at the entity
    virtual void Use(bool keydown) {
        if (!m_target.empty()) {
            std::vector<Entity*> targets = FindTargets(m_target);
            for (Entity* target : targets) {
                if (target->IsTriggerable()) {
                    switch (m_triggerType) {
                    case 0:
                        target->Trigger(!target->IsTriggered());
                        break;
                    case 1:
                        target->Trigger(true);
                        break;
                    case 2:
                        target->Trigger(false);
                        break;
                    }
                }
            }
        }
    }

    bool IsSpawned() const {return m_spawned;}

    // Called every frame
    virtual void FrameUpdate(float delta) {}
    // Called on a fixed delta after each physics update
    virtual void FixedUpdate(float delta) {}

protected:
    Model* m_model;
    std::string m_identifier;
    std::string m_target;
    int m_triggerType;
    bool m_triggerState;

private:
    EID m_id;
    bool m_spawned;
    Vector m_pos;
    Quaternion m_rot;
};

class EntityFactory {
public:
    using CreatorFunc = std::function<Entity*()>;

    static EntityFactory& Instance();
    void Register(std::string classname, CreatorFunc func);
    Entity* Create(std::string classname);

private:
    std::map<std::string, CreatorFunc> m_registry;
};

#define ENTCLASS(identifier, classname) \
    static struct classname##Registrar {\
        classname##Registrar() {\
            EntityFactory::Instance().Register(#identifier, []() {\
                return new classname();\
            });\
        }\
    } classname##_Registrar_Instance;