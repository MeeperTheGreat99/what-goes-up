#pragma once
#include "model.h"
#include "physdebugger.h"
#include <btBulletDynamicsCommon.h>

class Entity;

class PhysicsWorld {
public:
    struct TraceResult {
        float fraction = 0.0f;
        Vector startpoint;
        Vector endpoint;
        Vector contactpoint;
        Vector normal;
        Entity* entity = nullptr;
        btRigidBody* body = nullptr;
    };

    static btCollisionShape* ShapeFromMesh(Mesh* mesh, bool complex);
    static bool TraceLine(const Vector& start, const Vector& end, TraceResult& result, Entity* ignore = nullptr);
    static bool TraceShape(const Vector& start, const Vector& end, btCollisionShape* shape, TraceResult& result, Entity* ignore = nullptr);

    PhysicsWorld();
    ~PhysicsWorld();

    void Update(float delta);
    void SetDebugger(PhysDebugger* debugger);

    btDynamicsWorld* world;

private:
    btCollisionConfiguration* m_config;
    btCollisionDispatcher* m_dispatch;
    btDbvtBroadphase* m_broadphase;
    btConstraintSolver* m_solver;
};