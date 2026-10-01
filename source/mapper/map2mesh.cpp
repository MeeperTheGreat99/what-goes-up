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
            Vector edge1 = poly.vertices[1].point - poly.vertices[0].point;
            Vector edge2 = poly.vertices[2].point - poly.vertices[0].point;
            float duv1x = poly.vertices[1].uv[0] - poly.vertices[0].uv[0];
            float duv1y = poly.vertices[1].uv[1] - poly.vertices[0].uv[1];
            float duv2x = poly.vertices[2].uv[0] - poly.vertices[0].uv[0];
            float duv2y = poly.vertices[2].uv[1] - poly.vertices[0].uv[1];
            float det = duv1x * duv2y - duv1y * duv2x;
            float invDet = 1.0f / det;
            Vector tangent = ((edge1 * duv2y - edge2 * duv1y) * invDet).normalized();
            Vector bitangent = ((edge1 * -duv2x + edge2 * duv1x) * invDet).normalized();

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
                memcpy(&vertex.tangent, &tangent[0], sizeof(Vector));
                memcpy(&vertex.bitangent, &bitangent[0], sizeof(Vector));
                vertices.push_back(vertex);
                indices.push_back(index++);
            }
        }

        Material material;
        if (!Material::Load("res/materials/" + entry.first + ".mat", material)) {
            material.albedo.texture = Texture::Load("res/textures/" + entry.first + ".png", false, true);
            material.flags |= MATERIAL_FLAG_ALBEDO_TEXTURE;
        }
            
        parts.push_back(new Mesh(vertices, indices, material));
    }

    return new Model(parts);
}