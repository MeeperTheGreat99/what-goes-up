#include "plane.h"
#include "mapconvert.h"

Plane::Plane() {
    this->normal = 0.0;
    this->dist = 0.0;
}

Plane::Plane(Vector normal, double dist) {
    this->normal = normal.normalized();
    this->dist = dist;
}

Plane::Plane(Vector p0, Vector p1, Vector p2) {
    normal = (p0 - p1).cross(p2 - p1).normalized();
    dist = -normal.dot(p0);
}

double Plane::GetDistance(Vector p) {
    return normal.dot(p) + dist;
}

Plane::Classification Plane::Classify(Vector p) {
    double dist = GetDistance(p);

    if (dist > MapConvert::Epsilon) {
        return FRONT;
    } else if (dist < -MapConvert::Epsilon) {
        return BACK;
    }

    return ONPLANE;
}

bool Plane::ShouldIntersect(Plane plane0, Plane plane1) {
    if (1.0 - abs(normal.dot(plane0.normal)) < MapConvert::Epsilon) {
        return false;
    }

    if (1.0 - abs(normal.dot(plane1.normal)) < MapConvert::Epsilon) {
        return false;
    }

    if (1.0 - abs(plane0.normal.dot(plane1.normal)) < MapConvert::Epsilon) {
        return false;
    }

    return true;
}

bool Plane::GetIntersection(Plane plane0, Plane plane1, Vector& p) {
    double denom = normal.dot(plane0.normal.cross(plane1.normal));
    if (abs(denom) < MapConvert::Epsilon) {
        return false;
    }

    Vector term0 = plane0.normal.cross(plane1.normal) * -dist;
    Vector term1 = plane1.normal.cross(normal) * plane0.dist;
    Vector term2 = normal.cross(plane0.normal) * plane1.dist;

    p = (term0 - term1 - term2) / denom;
    
    return true;
}

bool Plane::GetIntersection(Vector start, Vector end, Vector& intersection, double& percent) {
    Vector dir = (end - start).normalized();

    double denom = normal.dot(dir);
    if (abs(denom) < MapConvert::Epsilon) {
        return false;
    }

    double num = -GetDistance(start);
    percent = num / denom;
    intersection = start + dir * percent;
    percent = percent / (end - start).length();

    return true;
}

Vector Plane::Project(Vector p) {
    double sigd = GetDistance(p);
    return p - normal * sigd;
}