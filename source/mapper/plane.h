#pragma once
#include "../vector.h"

class Plane {
public:
    enum Classification {FRONT, BACK, ONPLANE};
    Vector normal;
    double dist;

    Plane();
    Plane(Vector normal, double dist);
    Plane(Vector p0, Vector p1, Vector p2);

    double GetDistance(Vector p);
    Classification Classify(Vector p);
    bool ShouldIntersect(Plane plane0, Plane plane1);
    bool GetIntersection(Plane plane0, Plane plane1, Vector& p);
    bool GetIntersection(Vector start, Vector end, Vector& intersection, double& percent);
    Vector Project(Vector p);
};