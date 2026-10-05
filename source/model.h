#pragma once
#include "camera.h"
#include "material.h"
#include "shader.h"
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

    void Draw(ObjectShader* shader);
    Material GetMaterial() const { return m_material; }
    Vector GetCentroid() const { return m_centroid; }
    float GetRadius() const { return m_radius; }

    const std::vector<Vertex>& GetVertices() const { return m_vertices; }
    const std::vector<unsigned int>& GetIndices() const { return m_indices; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    unsigned int m_vao, m_vbo, m_ebo;
    Material m_material;
    Vector m_centroid;
    float m_radius;
};

class Model {
public:
    static void LoadErrorModel();
    static Model* LoadExternal(std::string filename);

    Model(const std::vector<Mesh*>& meshes);

    void Draw(ObjectShader* shader = nullptr, Camera::Frustum* frustum = nullptr);

    const std::vector<Mesh*>& GetMeshes() const { return m_meshes; }
    float GetRadius() const { return m_radius; }

private:
    struct ImportData {
        std::vector<Mesh*> meshes;
    };

    static void ProcessNode(const aiScene* scene, aiNode* node, ImportData& id);
    static Texture* FetchTexture(const aiScene* scene, aiMaterial* material, aiTextureType type);

    static Model* ErrorModel;

    std::vector<Mesh*> m_meshes;
    float m_radius;
};