#include "entity.h"

std::map<EID, Entity*> Entity::Entities = {};
PhysicsWorld* Entity::World = nullptr;
float Entity::WorldTime = 0.0f;

Entity* Entity::Create(std::string classname) {
    return EntityFactory::Instance().Create(classname);
}

EntityFactory& EntityFactory::Instance() {
    static EntityFactory instance;
    return instance;
}

void EntityFactory::Register(std::string classname, CreatorFunc func) {
    m_registry[classname] = func;
}

Entity* EntityFactory::Create(std::string classname) {
    if (m_registry.count(classname)) {
        return m_registry[classname]();
    }

    printf("unknown entity: %s\n", classname.c_str());

    return nullptr;
}