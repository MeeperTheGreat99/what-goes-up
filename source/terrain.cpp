#include "terrain.h"
#include <stb_image.h>

Terrain::Terrain(std::string filename) {
    m_image = stbi_load(filename.c_str(), &m_width, &m_height, &m_chan, 0);
    if (!m_image) {
        throw std::runtime_error("a terrain image failed to load");
    }

    m_grass = Texture::Load("res/textures/grass.jpg", true);
    m_dirt = Texture::Load("res/textures/dirt.jpg", true);
    m_rock = Texture::Load("res/textures/rock.jpg", true);

    memset(m_loadedChunks, 0, sizeof(m_loadedChunks));
}

Terrain::~Terrain() {
    for (Chunk* chunk : m_loadedChunks) {
        if (chunk) {
            delete chunk;
        }
    }

    delete m_rock;
    delete m_dirt;
    delete m_grass;

    stbi_image_free(m_image);
}

void Terrain::UpdateChunks(float x, float z) {
    int chunkX = roundf(x / kChunkSize - 0.5f);
    int chunkY = roundf(z / kChunkSize - 0.5f);

    for (int y = 0; y < kChunkLateralCount; y++) {
        for (int x = 0; x < kChunkLateralCount; x++) {
            bool shouldLoad = abs(y - chunkY) <= kChunkLoadRadius && abs(x - chunkX) <= kChunkLoadRadius;
            Chunk** chunkElement = &m_loadedChunks[y * kChunkLateralCount + x];
            if (shouldLoad && !*chunkElement) {
                LoadChunk(x, y);
            } else if (!shouldLoad && *chunkElement) {
                Chunk* chunk = *chunkElement;
                delete chunk->mesh;
                delete chunk;
                *chunkElement = nullptr;
            }
        }
    }
}

void Terrain::Draw() {
    m_grass->Use(0);
    m_dirt->Use(1);
    m_rock->Use(2);

    for (Chunk* chunk : m_loadedChunks) {
        if (!chunk) {
            continue;
        }

        chunk->mesh->Draw(true);
    }
}

float Terrain::SampleImage(int x, int y) {
    int xIndex = x;
    int yIndex = y;

    if (xIndex < 0) {
        xIndex = 0;
    }

    if (xIndex >= m_width) {
        xIndex = m_width - 1;
    }

    if (yIndex < 0) {
        yIndex = 0;
    }

    if (yIndex >= m_height) {
        yIndex = m_height - 1;
    }

    return m_image[(yIndex * m_width + xIndex) * m_chan] / 255.0f;
}

float Terrain::SampleImage(float x, float y, float resolution) {
    float xIndex = x / resolution * m_width;
    float yIndex = y / resolution * m_height;
    float tx = xIndex - roundf(xIndex);
    float ty = yIndex - roundf(yIndex);
    float val1 = SampleImage(xIndex, yIndex);
    float val2 = SampleImage(xIndex + 1, yIndex);
    float val3 = SampleImage(xIndex, yIndex + 1);
    float val4 = SampleImage(xIndex + 1, yIndex + 1);
    float top = val1 + (val2 - val1) * tx;
    float bottom = val3 + (val4 - val3) * tx;
    return top + (bottom - top) * ty;
}

void Terrain::LoadChunk(int chunkX, int chunkY) {
    std::vector<Vertex> finalVertices;
    std::vector<unsigned int> indices;
    int resCount = kResolution / kChunkLateralCount;
    int startY = chunkY * resCount;
    int startX = chunkX * resCount;

    float pixelSize = kMapSize / kResolution;
    for (int y = startY; y < startY + resCount; y++) {
        for (int x = startX; x < startX + resCount; x++) {
            Vertex vertices[4];
            Vector basePos = Vector(x, 0, y);
            vertices[0].position = basePos;
            vertices[0].uv[1] = 1.0f;
            vertices[1].position = basePos + Vector(0, 0, 1);
            vertices[2].position = basePos + Vector(1, 0, 1);
            vertices[2].uv[0] = 1.0f;
            vertices[3].position = basePos + Vector(1, 0, 0);
            vertices[3].uv[0] = 1.0f;
            vertices[3].uv[1] = 1.0f;

            unsigned int indexTable[6] = {
                0, 1, 2,
                2, 3, 0
            };

            int base = finalVertices.size();
            for (int i = 0; i < 4; i++) {
                vertices[i].position.y = kDepthConstant * SampleImage(vertices[i].position.x, vertices[i].position.z, kResolution);
                vertices[i].position.x *= pixelSize;
                vertices[i].position.z *= pixelSize;
            }

            Vector u = vertices[1].position - vertices[0].position;
            Vector w = vertices[2].position - vertices[0].position;
            Vector n1 = u.cross(w).normalized();
            u = vertices[2].position - vertices[0].position;
            w = vertices[3].position - vertices[0].position;
            Vector n2 = u.cross(w).normalized();
            Vector lerped = (n1 + (n2 - n1) * 0.5f).normalized();
            vertices[0].normal = lerped;
            vertices[1].normal = n1;
            vertices[2].normal = lerped;
            vertices[3].normal = n2;
            for (int i = 0; i < 4; i++) {
                finalVertices.push_back(vertices[i]);
            }

            for (int i = 0; i < 6; i++) {         
                indices.push_back(base + indexTable[i]);
            }
        }
    }

    Chunk* chunk = new Chunk();
    chunk->x = chunkX;
    chunk->y = chunkY;
    chunk->mesh = new Mesh(finalVertices, indices, Material());
    m_loadedChunks[chunkY * kChunkLateralCount + chunkX] = chunk;
}