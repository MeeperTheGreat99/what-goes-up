#pragma once
#include "texture.h"
#include "vector.h"

#define MATERIAL_FLAG_ALBEDO_TEXTURE (1 << 0)
#define MATERIAL_FLAG_SPECULAR_TEXTURE (1 << 1)

struct Material {
    unsigned char flags;
    union {
        Texture* texture;
        Vector color = 0.5f;
    } albedo;
    Texture* normal = nullptr;
    union {
        Texture* texture;
        float value = 0.5f;
    } specular;
};