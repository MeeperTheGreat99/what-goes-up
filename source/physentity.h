#pragma once
#include "entity.h"
#include <glm/ext/scalar_constants.hpp>
#include <btBulletDynamicsCommon.h>

class PhysEntity : public Entity {
public:
    ~PhysEntity() {
        DestroyRigidbody();
    }

    Vector GetMotionStatePos() const {
        btTransform transform;
        m_motionState->getWorldTransform(transform);
        return transform.getOrigin();
    }

    Vector GetMotionStateAngles() const {
        btTransform transform;
        m_motionState->getWorldTransform(transform);
        return Quaternion(transform.getRotation()).toEulerAngles();
    }

    virtual void SetPos(Vector pos) override {
        Entity::SetPos(pos);
        if (m_rigidbody) {
            btTransform transform = m_rigidbody->getWorldTransform();
            transform.setOrigin(pos.bt());
            m_rigidbody->setWorldTransform(transform);
        }
    }
    virtual Vector GetPos() const override {
        if (m_rigidbody) {
            btTransform transform = m_rigidbody->getWorldTransform();
            return transform.getOrigin();
        } else {
            return Entity::GetPos();
        }
    }

    virtual void SetRot(Quaternion rot) override {
        Entity::SetRot(rot);
        if (m_rigidbody) {
            btTransform transform = m_rigidbody->getWorldTransform();
            transform.setRotation(rot.bt());
            m_rigidbody->setWorldTransform(transform);
        }
    }
    virtual Quaternion GetRot() const override {
        if (m_rigidbody) {
            btTransform transform = m_rigidbody->getWorldTransform();
            return transform.getRotation();
        } else {
            return Entity::GetRot();
        }
    }

    virtual void Spawn() override {
        Entity::Spawn();

        if (m_rigidbody) {
            World->world->addRigidBody(m_rigidbody);
        }
    }

    virtual void Clean() override {
        Entity::Clean();

        if (m_rigidbody) {
            World->world->removeRigidBody(m_rigidbody);
        }
    }

protected:
    btMotionState* m_motionState = nullptr;
    btRigidBody* m_rigidbody = nullptr;

    void InitializeRigidbody(btCollisionShape* shape, float mass) {
        btVector3 inertia;
        
        DestroyRigidbody();

        shape->calculateLocalInertia(mass, inertia);
        m_motionState = new btDefaultMotionState();
        
        btTransform transform;
        transform.setOrigin(GetPos().bt());
        transform.setRotation(GetRot().bt());

        m_rigidbody = new btRigidBody(mass, m_motionState, shape, inertia);
        m_rigidbody->setUserPointer(this);
        m_rigidbody->setWorldTransform(transform);

        if (IsSpawned()) {
            World->world->addRigidBody(m_rigidbody);
        }
    }

    void DestroyRigidbody() {
        if (m_rigidbody) {
            if (IsSpawned()) {
                World->world->removeRigidBody(m_rigidbody);
            }

            delete m_rigidbody;
            m_rigidbody = nullptr;
            delete m_motionState;
            m_motionState = nullptr;
        }
    }
};