#pragma once
#include "material.h"
#include "vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>

class Mesh {
public:
    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        Material material
    );
    ~Mesh();

    void Draw(bool ignoreMat);

private:
    std::vector<unsigned int> m_indices;
    unsigned int m_vao, m_vbo, m_ebo;
    Material m_material;
};

class Model {
public:
    static Model* LoadExternal(std::string filename);

    Model(const std::vector<Mesh*>& meshes);

    void Draw(bool ignoreMat = false);

private:
    struct ImportData {
        std::vector<Mesh*> meshes;
    };

    static void ProcessNode(const aiScene* scene, aiNode* node, ImportData& id);
    static Texture* FetchTexture(const aiScene* scene, aiMaterial* material, aiTextureType type);

    std::vector<Mesh*> m_meshes;
};