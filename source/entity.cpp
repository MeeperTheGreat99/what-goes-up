#include "entity.h"

std::map<EID, Entity*> Entity::Entities = {};
PhysicsWorld* Entity::World = nullptr;
float Entity::WorldTime = 0.0f;