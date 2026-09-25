#pragma once
#include <btBulletDynamicsCommon.h>

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    void Update(float delta);

    btDynamicsWorld* world;

private:
    btCollisionConfiguration* m_config;
    btCollisionDispatcher* m_dispatch;
    btDbvtBroadphase* m_broadphase;
    btConstraintSolver* m_solver;
};