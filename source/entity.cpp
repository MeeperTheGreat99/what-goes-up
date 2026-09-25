#include "entity.h"

std::map<EID, Entity*> Entity::Entities = {};
PhysicsWorld* Entity::World = nullptr;