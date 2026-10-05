#pragma once
#include "../vector.h"

class Plane {
public:
    enum Classification {FRONT, BACK, ONPLANE};
    Vector normal;
    float dist;

    Plane();
    Plane(Vector normal, float dist);
    Plane(glm::vec4 vec);
    Plane(Vector point, Vector normal);
    Plane(Vector p0, Vector p1, Vector p2);

    float GetDistance(Vector p);
    Classification Classify(Vector p);
    bool ShouldIntersect(Plane plane0, Plane plane1);
    bool GetIntersection(Plane plane0, Plane plane1, Vector& p);
    bool GetIntersection(Vector start, Vector end, Vector& intersection, float& percent);
    Vector Project(Vector p);
};