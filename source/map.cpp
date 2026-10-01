#include "map.h"
#include "entity.h"
#include "mapper/mapconvert.h"
#include "mapper/mapent.h"
#include "mapper/map2mesh.h"
#include "world.h"
#include <fstream>

std::string Map::CurrentMap = "";
Vector Map::PlayerPosition;
Vector Map::PlayerAngles;

void Map::Load(std::string name) {
    if (!CurrentMap.empty()) {
        Unload();
    }
    
    if (name.empty()) {
        return;
    }

    std::string filename = "res/maps/" + name + ".map";
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        printf("couldn't load map: %s\n", filename.c_str());
        return;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    char* data = new char[size];
    file.read((char*)data, size);
    file.close();

    MapConvert* converter = new MapConvert(data, size);
    delete[] data;

    if (!converter->Convert()) {
        printf("couldn't parse map: %s\n", filename.c_str());
        return;
    }

    std::vector<MapEnt*> entData = converter->GetEntities();
    for (MapEnt* ent : entData) {
        std::string classname = ent->properties["classname"];
        if (classname == "light") {
            Light* light = new Light();
            light->SetPos(Vector::fromOrigin(ent->properties["origin"]));
            light->SetColor(Vector::fromAngles(ent->properties["color"]));
            light->SetIntensity(std::stof(ent->properties["intensity"]));
            continue;
        } else if (classname == "info_player_start") {
            PlayerPosition = Vector::fromOrigin(ent->properties["origin"]);
            PlayerAngles = Vector::fromAngles(ent->properties["angles"]);
            continue;
        }

        Entity* entity = Entity::Create(classname);
        if (!entity) {
            continue;
        }

        for (auto& prop : ent->properties) {
            entity->ApplyProperty(prop.first, prop.second);
        }

        if (!ent->polys.empty()) {
            Model* model = MapEntToMesh(ent);
            if (model) {
                entity->m_model = model;
            }

            typedef std::vector<Vector> Collision;
            std::vector<Collision> collisions;

            for (auto& shape : ent->collisions) {
                Collision collision;

                for (size_t ID : shape) {
                    int index = 0;

                    auto find = std::find_if(ent->polys.begin(), ent->polys.end(), [&](Poly& a) {
                        index = 0;

                        for (MapConvert::Vertex& v : a.vertices) {
                            if (v.ID == ID) {
                                return true;
                            }
                            
                            index++;
                        }

                        return false;
                    });

                    if (find != ent->polys.end()) {
                        Poly& poly = ent->polys[find - ent->polys.begin()];
                        Vector point = poly.vertices[index].point;
                        collision.push_back(Vector(point.x, point.y, point.z));
                    }
                }

                collisions.push_back(collision);
            }

            World* world = dynamic_cast<World*>(entity);
            if (world) {
                if (collisions.size() < 2) {
                    btConvexHullShape* shape = new btConvexHullShape();

                    for (Vector p : collisions[0]) {
                        shape->addPoint(p.bt(), false);
                    }

                    shape->recalcLocalAabb();
                    world->AssumeShape(shape);
                } else {
                    btTransform transform;
                    btCompoundShape* compound = new btCompoundShape();
                    
                    transform.setIdentity();

                    for (Collision& collision : collisions) {
                        btConvexHullShape* shape = new btConvexHullShape();

                        for (Vector p : collision) {
                            shape->addPoint(p.bt(), false);
                        }

                        shape->recalcLocalAabb();
                        compound->addChildShape(transform, shape);
                    }

                    compound->recalculateLocalAabb();
                    world->AssumeShape(compound);
                }
            }
        }
    }

    for (auto& entry : Entity::Entities) {
        entry.second->Spawn();
    }

    CurrentMap = name;
}

void Map::Unload() {
    if (CurrentMap.empty()) {
        return;
    }

    auto& toRemove = Entity::Entities;
    for (auto& entry : toRemove) {
        Entity* entity = entry.second;
        entity->Clean();
        delete entity;
    }

    CurrentMap = "";
}