#pragma once
#include "poly.h"
#include <vector>

class Brush {
public:
    Vector min, max;
    std::vector<Poly> polys;
    std::vector<size_t> collision;

    void ClipToBrush(Brush& brush, bool clipOnPlane);
    void CalculateAABB();
    bool AABBIntersect(Brush& brush);

    static std::vector<Poly> MergeList(std::vector<Brush>& brushes);
};