#pragma once
#include "model.h"
#include <string>

class Terrain {
public:
    static constexpr float kMapSize = 4000.0f;
    static constexpr float kDepthConstant = 0.001f * kMapSize;
    static constexpr int kResolution = 8192;
    static constexpr int kChunkSize = 10;
    static constexpr int kChunkLoadRadius = 8;
    static constexpr int kChunkLateralCount = kMapSize / kChunkSize;

    Terrain(std::string filename);
    ~Terrain();

    void UpdateChunks(float x, float y);
    void Draw();
    float SampleImage(int x, int y);
    float SampleImage(float x, float y, float resolution);

private:
    struct Chunk {
        int x, y;
        Mesh* mesh;
    };

    unsigned char* m_image;
    int m_width, m_height, m_chan;
    Texture* m_grass;
    Texture* m_dirt;
    Texture* m_rock;
    Chunk* m_loadedChunks[kChunkLateralCount * kChunkLateralCount];

    void LoadChunk(int chunkX, int chunkY);
};