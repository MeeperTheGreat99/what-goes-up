#include "entity.h"
#include <btBulletDynamicsCommon.h>

class PhysEntity : public Entity {
public:
    PhysEntity(btCollisionShape* shape, float mass) {
        m_motionState = new btDefaultMotionState();
        btVector3 inertia;
        shape->calculateLocalInertia(mass, inertia);
        m_rigidbody = new btRigidBody(mass, m_motionState, shape, inertia);
        World->world->addRigidBody(m_rigidbody);
    }

    ~PhysEntity() {
        World->world->removeRigidBody(m_rigidbody);
        delete m_rigidbody;
        delete m_motionState;
    }

    virtual void SetPos(Vector pos) override {
        Entity::SetPos(pos);
        btTransform transform = m_rigidbody->getWorldTransform();
        transform.setOrigin(pos.bt());
        m_rigidbody->setWorldTransform(transform);
        m_motionState->setWorldTransform(transform);
    }

    virtual void SetAngles(Vector angles) override {
        Entity::SetAngles(angles);
        btTransform transform = m_rigidbody->getWorldTransform();
        transform.setRotation(btQuaternion(
            glm::radians(angles.y),
            glm::radians(angles.x),
            glm::radians(angles.z)
        ));
        m_rigidbody->setWorldTransform(transform);
        m_motionState->setWorldTransform(transform);
    }

protected:
    btMotionState* m_motionState;
    btRigidBody* m_rigidbody;
};