#pragma once
#include "vector.h"

struct Vertex {
    Vector position;
    Vector normal;
    float uv[2] = {0.0f, 0.0f};
    unsigned char bone = 0;
};