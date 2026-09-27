#include "brush.h"

void Brush::ClipToBrush(Brush& brush, bool clipOnPlane) {
    std::vector<Poly> clippedPolys = {};

    for (Poly& poly : polys) {
        auto clipped = Poly::ClipToList(brush.polys, poly, clipOnPlane, 0);
        if (!clipped.empty()) {   
            clippedPolys.insert(clippedPolys.end(), clipped.begin(), clipped.end());
        }
    }

    polys = clippedPolys;
}

void Brush::CalculateAABB() {
    min = polys.at(0).vertices.at(0).point;
    max = polys.at(0).vertices.at(0).point;

    for (Poly& poly : polys) {
        for (MapConvert::Vertex& vertex : poly.vertices) {
            min.x = std::min(min.x, vertex.point.x);
            min.y = std::min(min.y, vertex.point.y);
            min.z = std::min(min.z, vertex.point.z);
            max.x = std::max(max.x, vertex.point.x);
            max.y = std::max(max.y, vertex.point.y);
            max.z = std::max(max.z, vertex.point.z);
        }
    }
}

bool Brush::AABBIntersect(Brush& brush) {
    if (min.x > brush.max.x || brush.min.x > max.x) {
        return false;
    }

    if (min.y > brush.max.y || brush.min.y > max.y) {
        return false;
    }

    if (min.z > brush.max.z || brush.min.z > max.z) {
        return false;
    }

    return true;
}

std::vector<Poly> Brush::MergeList(std::vector<Brush>& brushes) {
    std::vector<Brush> clipped = brushes;

    for (int i = 0; i < brushes.size(); i++) {
        bool clipOnPlane = false;
        for (int k = 0; k < brushes.size(); k++) {
            if (i == k) {
                clipOnPlane = true;
            } else if (clipped[i].AABBIntersect(brushes[k])) {
                clipped[i].ClipToBrush(brushes[k], clipOnPlane);
            }
        }
    }

    std::vector<Poly> retpolys = {};
    for (int i = 0; i < clipped.size(); i++) {
        for (auto& poly : clipped[i].polys) {
            retpolys.push_back(poly);
        }
    }

    return retpolys;
}