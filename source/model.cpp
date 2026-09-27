#include "model.h"
#include "gl.h"
#include <assimp/postprocess.h>

Model* Model::ErrorModel = nullptr;

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    Material material
) : m_vertices(vertices), m_indices(indices), m_material(material) {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Mesh::Draw(ObjectShader* shader) {
    if (m_material.flags & MATERIAL_FLAG_ALBEDO_TEXTURE) {
        m_material.albedo.texture->Use(0);
    } else {
        Texture::GetWhite()->Use(0);
        shader->SetAlbedoColor(m_material.albedo.color.gl());
    }
    
    if (m_material.normal) {
        m_material.normal->Use(1);
    }
    
    if (m_material.flags & MATERIAL_FLAG_SPECULAR_TEXTURE) {
        m_material.specular.texture->Use(2);
    }

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

void Model::LoadErrorModel() {
    ErrorModel = LoadExternal("res/models/error.obj");
    if (!ErrorModel) {
        throw std::runtime_error("could not load error model");
    }
}

Model* Model::LoadExternal(std::string filename) {
    Assimp::Importer* imp = new Assimp::Importer();

    const aiScene* scene = imp->ReadFile(
        filename.c_str(),
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_EmbedTextures
    );

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        delete imp;
        return ErrorModel;
    }

    ImportData id;
    ProcessNode(scene, scene->mRootNode, id);

    delete imp;

    return new Model(id.meshes);
}

Model::Model(const std::vector<Mesh*>& meshes) : m_meshes(meshes) {}

void Model::Draw(ObjectShader* shader) {
    for (Mesh* mesh : m_meshes) {
        mesh->Draw(shader);
    }
}

void Model::ProcessNode(const aiScene* scene, aiNode* node, ImportData& id) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Vertex> vertices;
        for (int k = 0; k < mesh->mNumVertices; k++) {
            Vertex vertex;
            
            memcpy(&vertex.position[0], &mesh->mVertices[k][0], sizeof(Vector));
            
            if (mesh->HasNormals()) {
                memcpy(&vertex.normal[0], &mesh->mNormals[k][0], sizeof(Vector));
            }

            if (mesh->HasTextureCoords(0)) {
                memcpy(&vertex.uv[0], &mesh->mTextureCoords[0][k].x, sizeof(float) * 2);
            }

            vertex.uv[1] *= -1.0f;
            vertices.push_back(vertex);
        }
        
        std::vector<unsigned int> indices;
        for (int k = 0; k < mesh->mNumFaces; k++) {
            aiFace* face = &mesh->mFaces[k];
            for (int j = 0; j < face->mNumIndices; j++) {
                indices.push_back(face->mIndices[j]);
            }
        }

        Material mat;

        Texture* albedo = FetchTexture(scene, material, aiTextureType_DIFFUSE);
        if (albedo) {
            mat.albedo.texture = albedo;
            mat.flags |= MATERIAL_FLAG_ALBEDO_TEXTURE;
        } else {
            aiColor4D color;
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
                mat.albedo.color = Vector(color.r, color.g, color.b);
            } else {
                mat.albedo.texture = Texture::GetDefault();
                mat.flags |= MATERIAL_FLAG_ALBEDO_TEXTURE;
            }
        }

        id.meshes.push_back(new Mesh(vertices, indices, mat));
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(scene, node->mChildren[i], id);
    }
}

Texture* Model::FetchTexture(const aiScene* scene, aiMaterial* material, aiTextureType type) {
    bool srgb = type == aiTextureType_DIFFUSE;

    aiString texpath;
    if (material->GetTexture(type, 0, &texpath) == aiReturn_SUCCESS) {
        const aiTexture* texture = scene->GetEmbeddedTexture(texpath.C_Str());
        if (texture) {
            if (!texture->mHeight) {
                return nullptr;//Texture::LoadMemory((unsigned char*)texture->pcData, texture->mWidth, srgb);
            } else {
                unsigned char* data = new unsigned char[texture->mWidth * texture->mHeight * 4];
                for (int i = 0; i < texture->mWidth * texture->mHeight; i++) {
                    int idx = i << 2;
                    data[idx] = texture->pcData[i].r;
                    data[idx+1] = texture->pcData[i].g;
                    data[idx+2] = texture->pcData[i].b;
                    data[idx+3] = texture->pcData[i].a;
                }
                Texture* created = new Texture(texture->mWidth, texture->mHeight, 4, data, srgb);
                delete[] data;
                return created;
            }
        } else {
            return Texture::Load(texpath.C_Str(), srgb);
        }
    }

    return nullptr;
}