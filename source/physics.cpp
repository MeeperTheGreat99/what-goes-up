#include "physics.h"

PhysicsWorld::PhysicsWorld() {
    m_config = new btDefaultCollisionConfiguration();
    m_dispatch = new btCollisionDispatcher(m_config);
    m_broadphase = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver();
    world = new btDiscreteDynamicsWorld(m_dispatch, m_broadphase, m_solver, m_config);
    world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
}

PhysicsWorld::~PhysicsWorld() {
    delete world;
    delete m_solver;
    delete m_broadphase;
    delete m_dispatch;
    delete m_config;
}

void PhysicsWorld::Update(float delta) {
    world->stepSimulation(delta, 0, delta);
}