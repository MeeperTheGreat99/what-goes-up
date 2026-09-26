#include "physics.h"
#include "entity.h"

btCollisionShape* PhysicsWorld::ShapeFromMesh(Mesh* mesh, bool complex) {
    if (complex) {
        btTriangleMesh* triMesh = new btTriangleMesh();
        const std::vector<Vertex>& vertices = mesh->GetVertices();
        const std::vector<unsigned int>& indices = mesh->GetIndices();
        for (size_t i = 0; i < indices.size(); i += 3) {
            const Vertex& v0 = vertices[indices[i]];
            const Vertex& v1 = vertices[indices[i + 1]];
            const Vertex& v2 = vertices[indices[i + 2]];
            triMesh->addTriangle(v0.position.bt(), v1.position.bt(), v2.position.bt());
        }
        btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triMesh, true);
        shape->setUserPointer(triMesh);
        return shape;
    } else {
        const std::vector<Vertex>& vertices = mesh->GetVertices();
        btConvexHullShape* shape = new btConvexHullShape();
        shape->setMargin(0.01f);
        for (const Vertex& v : vertices) {
            shape->addPoint(v.position.bt(), false);
        }
        shape->recalcLocalAabb();
        return shape;
    }
}

btCollisionShape* PhysicsWorld::ShapeFromModel(Model* model, bool complex) {
    btCompoundShape* compound = new btCompoundShape();
        for (Mesh* mesh : model->GetMeshes()) {
            btCollisionShape* shape = ShapeFromMesh(mesh, complex);
            if (shape) {
                btTransform transform;
                transform.setIdentity();
                compound->addChildShape(transform, shape);
            }
        }
        return compound;
}

void PhysicsWorld::SafeDeleteShape(btCollisionShape* shape) {
    if (!shape) return;

    btCompoundShape* compound = dynamic_cast<btCompoundShape*>(shape);
    if (compound) {
        for (int i = 0; i < compound->getNumChildShapes(); i++) {
            SafeDeleteShape(compound->getChildShape(i));
        }
    }

    btBvhTriangleMeshShape* triMeshShape = dynamic_cast<btBvhTriangleMeshShape*>(shape);
    if (triMeshShape) {
        btTriangleMesh* triMesh = static_cast<btTriangleMesh*>(triMeshShape->getUserPointer());
        delete triMesh;
    }

    delete shape;
}

bool PhysicsWorld::TraceLine(const Vector& start, const Vector& end, TraceResult& result, Entity* ignore) {
    btCollisionWorld::ClosestRayResultCallback rayCallback(start.bt(), end.bt());
    rayCallback.m_collisionFilterMask = btBroadphaseProxy::AllFilter;
    rayCallback.m_collisionFilterGroup = btBroadphaseProxy::AllFilter;

    Entity::World->world->rayTest(start.bt(), end.bt(), rayCallback);

    if (rayCallback.hasHit()) {
        result.fraction = rayCallback.m_closestHitFraction;
        result.startpoint = start;
        result.endpoint = start + (end - start) * result.fraction;
        result.contactpoint = result.endpoint;
        result.normal = Vector(rayCallback.m_hitNormalWorld);
        result.body = (btRigidBody*)rayCallback.m_collisionObject;
        result.entity = (Entity*)result.body->getUserPointer();
        
        if (result.entity == ignore) {
            return false;
        }

        return true;
    }

    return false;
}

bool PhysicsWorld::TraceShape(const Vector& start, const Vector& end, btCollisionShape* shape, TraceResult& result, Entity* ignore) {
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(start.bt());

    btTransform endTransform;
    endTransform.setIdentity();
    endTransform.setOrigin(end.bt());

    btCollisionWorld::ClosestConvexResultCallback convexCallback(start.bt(), end.bt());
    convexCallback.m_collisionFilterMask = btBroadphaseProxy::AllFilter;
    convexCallback.m_collisionFilterGroup = btBroadphaseProxy::AllFilter;

    Entity::World->world->convexSweepTest((btConvexShape*)shape, startTransform, endTransform, convexCallback);

    if (convexCallback.hasHit()) {
        result.fraction = convexCallback.m_closestHitFraction;
        result.startpoint = start;
        result.endpoint = start + (end - start) * result.fraction;
        result.contactpoint = convexCallback.m_hitPointWorld;
        result.normal = Vector(convexCallback.m_hitNormalWorld);
        result.body = (btRigidBody*)convexCallback.m_hitCollisionObject;
        result.entity = (Entity*)result.body->getUserPointer();
        
        if (result.entity == ignore) {
            return false;
        }

        return true;
    }

    return false;
}

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

void PhysicsWorld::SetDebugger(PhysDebugger* debugger) {
    debugger->setDebugMode(
        btIDebugDraw::DBG_DrawWireframe |
        btIDebugDraw::DBG_DrawText |
        btIDebugDraw::DBG_DrawFeaturesText |
        btIDebugDraw::DBG_DrawNormals
    );
    world->setDebugDrawer(debugger);
}