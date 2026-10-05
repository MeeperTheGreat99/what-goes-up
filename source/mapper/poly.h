#pragma once
#include "mapconvert.h"
#include "plane.h"

class Poly {
public:
    enum Classification {FRONT, SPLIT, BACK, ONPLANE};
    std::vector<MapConvert::Vertex> vertices;
    Plane plane;
    std::string texture;

    const bool operator==(const Poly &other) const;

    bool CalculatePlane();
    void SortVerticesCCW();
    void CalculateUVs(int width, int height, Plane axes[2], float scales[2]);
    void SplitPoly(Poly poly, Poly& front, Poly& back);
    Classification ClassifyPoly(Poly& poly);
    std::vector<Poly> Triangulate();
    
    static bool GetInteriorPoint(std::vector<MapConvert::Face>& faces, Vector& ret);
    static std::vector<Poly> GetPolysFromFaces(std::vector<MapConvert::Face>& faces);
    static std::vector<Poly> ClipToList(std::vector<Poly>& polys, Poly poly, bool clipOnPlane, int idx);
};