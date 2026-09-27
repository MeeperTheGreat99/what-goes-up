#include "../model.h"
#include "mapent.h"
#include "poly.h"

using namespace std;

Model* MapEntToMesh(MapEnt* ent) {
    map<string, vector<Poly>> partsToCreate = {};
    for (auto& poly : ent->polys) {
        if (!partsToCreate.count(poly.texture)) {
            partsToCreate[poly.texture] = {};
        }

        partsToCreate[poly.texture].push_back(poly);
    }

    vector<Mesh*> parts = {};
    for (auto entry : partsToCreate) {
        vector<Vertex> vertices {};
        vector<unsigned int> indices {};
        
        unsigned int index = 0;
        for (auto& poly : entry.second) {
            for (auto& v : poly.vertices) {
                Vertex vertex;
                vertex.position[0] = v.point.x;
                vertex.position[1] = v.point.y;
                vertex.position[2] = v.point.z;
                vertex.normal[0] = poly.plane.normal.x;
                vertex.normal[1] = poly.plane.normal.y;
                vertex.normal[2] = poly.plane.normal.z;
                vertex.uv[0] = v.uv[0];
                vertex.uv[1] = v.uv[1];
                vertices.push_back(vertex);
                indices.push_back(index++);
            }
        }

        Material material;
        if (!Material::Load("res/materials/" + entry.first + ".mat", material)) {
            material.albedo.texture = Texture::GetDefault();
            material.flags |= MATERIAL_FLAG_ALBEDO_TEXTURE;
        }
            
        parts.push_back(new Mesh(vertices, indices, material));
    }

    return new Model(parts);
}