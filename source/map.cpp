#include "map.h"
#include "entity.h"
#include "mapper/mapconvert.h"
#include "mapper/mapent.h"
#include "mapper/map2mesh.h"
#include <fstream>

std::string Map::CurrentMap = "";

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