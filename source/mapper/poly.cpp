#include "poly.h"
#include <algorithm>

const bool Poly::operator==(const Poly &other) const {
    if (vertices.size() == other.vertices.size()) {
        if (plane.dist == other.plane.dist) {
            if (plane.normal == other.plane.normal) {
                for (int i = 0; i < vertices.size(); i++) {
                    MapConvert::Vertex v0 = vertices.at(i);
                    MapConvert::Vertex v1 = other.vertices.at(i);
                    if (v0.point == v1.point) {
                        if (v0.uv[0] != v1.uv[0]) {
                            return false;
                        }

                        if (v0.uv[1] != v1.uv[1]) {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }

                if (texture == other.texture) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Poly::CalculatePlane() {
    if (vertices.size() < 3) {
        printf("Polygon has less than 3 vertices.\n");
        return false;
    }

    Vector centerOfMass(0.0);
    plane.normal = 0.0;

    for (int i = 0; i < vertices.size(); i++) {
        int k = i + 1;

        if (k >= vertices.size()) {
            k = 0;
        }

        plane.normal.x +=
            (vertices.at(i).point.y - vertices.at(k).point.y) *
            (vertices.at(i).point.z + vertices.at(k).point.z);
        plane.normal.y +=
            (vertices.at(i).point.z - vertices.at(k).point.z) *
            (vertices.at(i).point.x + vertices.at(k).point.x);
        plane.normal.z +=
            (vertices.at(i).point.x - vertices.at(k).point.x) *
            (vertices.at(i).point.y + vertices.at(k).point.y);

        centerOfMass.x += vertices.at(i).point.x;
        centerOfMass.y += vertices.at(i).point.y;
        centerOfMass.z += vertices.at(i).point.z;
    }

    if (abs(plane.normal.x) < MapConvert::Epsilon &&
        abs(plane.normal.y) < MapConvert::Epsilon &&
        abs(plane.normal.z) < MapConvert::Epsilon
    ) {
        return false;
    }

    float length = plane.normal.length();
    if (length < MapConvert::Epsilon) {
        return false;
    }

    plane.normal = plane.normal.normalized();

    float invCount = 1.0 / (float)vertices.size();
    centerOfMass.x *= invCount;
    centerOfMass.y *= invCount;
    centerOfMass.z *= invCount;

    plane.dist = -centerOfMass.dot(plane.normal);

    return true;
}

void Poly::SortVerticesCCW() {
    Vector center(0.0);
    for (int i = 0; i < vertices.size(); i++) {
        center += vertices.at(i).point;
    }
    center /= (float)vertices.size();

    Vector u = (vertices[0].point - center).normalized();
    Vector n = plane.normal.normalized();
    Vector v = n.cross(u);

    std::sort(vertices.begin(), vertices.end(), [&](const auto& a, const auto& b) {
        Vector l = a.point - center;
        Vector r = b.point - center;
        float angleL = atan2(l.dot(v), l.dot(u));
        float angleR = atan2(r.dot(v), r.dot(u));
        return angleL < angleR;
    });

    CalculatePlane();
    if (plane.normal.dot(n) < 0.0) {
        std::reverse(vertices.begin(), vertices.end());
        CalculatePlane();
    }
}

void Poly::CalculateUVs(int width, int height, Plane axes[2], float scales[2]) {
    for (int i = 0; i < vertices.size(); i++) {
        Vector point = vertices[i].point;

        float u = axes[0].normal.dot(point) / scales[0];
        u = (u + axes[0].dist) / (float)width;

        float v = axes[1].normal.dot(point) / scales[1];
        v = (v + axes[1].dist) / (float)height;

        vertices[i].uv[0] = u;
        vertices[i].uv[1] = v;
    }
}

void Poly::SplitPoly(Poly poly, Poly& front, Poly& back) {
    Plane::Classification classes[poly.vertices.size()];
    for (int i = 0; i < poly.vertices.size(); i++) {
        classes[i] = plane.Classify(poly.vertices.at(i).point);
    }

    front = Poly();
    back = Poly();

    front.texture = poly.texture;
    back.texture = poly.texture;
    front.plane = poly.plane;
    back.plane = poly.plane;

    for (int i = 0; i < poly.vertices.size(); i++) {
        MapConvert::Vertex vertex = poly.vertices[i];
        switch (classes[i]) {
        case Plane::FRONT:
            front.vertices.push_back(vertex);
            break;
        case Plane::BACK:
            back.vertices.push_back(vertex);
            break;
        case Plane::ONPLANE:
            front.vertices.push_back(vertex);
            back.vertices.push_back(vertex);
            break;
        }

        int next = i + 1;
        bool ignore = false;

        if (i == poly.vertices.size() - 1) {
            next = 0;
        }

        if (classes[i] == Plane::ONPLANE && classes[next] != Plane::ONPLANE) {
            ignore = true;
        } else if (classes[next] == Plane::ONPLANE && classes[i] != Plane::ONPLANE) {
            ignore = true;
        }

        if (!ignore && classes[i] != classes[next]) {
            MapConvert::Vertex v;
            float p;

            plane.GetIntersection(
                poly.vertices.at(i).point,
                poly.vertices.at(next).point,
                v.point, p
            );

            v.uv[0] = poly.vertices.at(next).uv[0] - poly.vertices.at(i).uv[0];
            v.uv[1] = poly.vertices.at(next).uv[1] - poly.vertices.at(i).uv[1];

            v.uv[0] = poly.vertices.at(i).uv[0] + p * v.uv[0];
            v.uv[1] = poly.vertices.at(i).uv[1] + p * v.uv[1];

            v.ID = vertex.ID;
            front.vertices.push_back(v);
            back.vertices.push_back(v);
        }
    }

    front.CalculatePlane();
    back.CalculatePlane();
}

Poly::Classification Poly::ClassifyPoly(Poly& poly) {
    bool front = false;
    bool back = false;
    float dist;

    for (int i = 0; i < poly.vertices.size(); i++) {
        dist = plane.normal.dot(poly.vertices.at(i).point) + plane.dist;
        if (dist > 0.001) {
            if (back) {
                return SPLIT;
            }

            front = true;
        } else if (dist < -0.001) {
            if (front) {
                return SPLIT;
            }

            back = true;
        }
    }

    if (front) {
        return FRONT;
    } else if (back) {
        return BACK;
    }

    return ONPLANE;
}

std::vector<Poly> Poly::Triangulate() {
    if (vertices.size() <= 3) {
        return {*this};
    }

    std::vector<Poly> polys = {};
    std::vector<MapConvert::Vertex> verts = vertices;
    while (verts.size() >= 3) {
        Poly poly;
        poly.vertices.push_back(verts[0]);
        poly.vertices.push_back(verts[1]);
        poly.vertices.push_back(verts[2]);
        poly.plane = plane;
        poly.texture = texture;
        polys.push_back(poly);
        verts.erase(verts.begin() + 1);
    }

    return polys;
}

bool Poly::GetInteriorPoint(std::vector<MapConvert::Face>& faces, Vector& ret) {
    for (int i = 0; i < faces.size(); i++) {
        for (int k = 0; k < faces.size(); k++) {
            for (int j = 0; j < faces.size(); j++) {
                Vector p;
                if (!faces[i].plane.GetIntersection(faces[k].plane, faces[j].plane, p)) {
                    continue;
                }

                bool inside = true;
                for (auto& face : faces) {
                    if (face.plane.Classify(p) == Plane::FRONT) {
                        inside = false;
                        break;
                    }
                }

                if (inside) {
                    ret = p;
                    return true;
                }
            }
        }
    }

    return false;
}

std::vector<Poly> Poly::GetPolysFromFaces(std::vector<MapConvert::Face>& faces) {
    const int faceCount = faces.size();
    std::vector<Poly> polys(faceCount);

    for (int i = 0; i < faceCount; i++) {
        polys[i].plane = faces[i].plane;
        for (int k = 0; k < faceCount; k++) {
            for (int j = 0; j < faceCount; j++) {
                if (i == k || i == j || k == j) {
                    continue;
                }
                
                MapConvert::Face& face0 = faces[i];
                MapConvert::Face& face1 = faces[k];
                MapConvert::Face& face2 = faces[j];

                if (!face0.plane.ShouldIntersect(face1.plane, face2.plane)) {
                    continue;
                }

                Vector p;
                if (face0.plane.GetIntersection(face1.plane, face2.plane, p)) {
                    for (int l = 0; l < faceCount; l++) {
                        if (faces[l].plane.Classify(p) == Plane::FRONT) {
                            break;
                        }

                        if (l == faceCount - 1) {
                            bool dupe = false;
                            for (MapConvert::Vertex& v : polys.at(i).vertices) {
                                if ((v.point - p).length2() < MapConvert::BigEpsilon) {
                                    dupe = true;
                                    break;
                                }
                            }
                            
                            if (!dupe) {
                                MapConvert::Vertex v;
                                v.point = p;
                                v.ID = MapConvert::VertexID++;
                                polys.at(i).vertices.push_back(v);
                            }
                        }
                    }
                }
            }
        }
    }

    return polys;
}

std::vector<Poly> Poly::ClipToList(std::vector<Poly>& polys, Poly poly, bool clipOnPlane, int idx) {
    Poly* me = &polys.at(idx);
    switch (me->ClassifyPoly(poly)) {
    case FRONT:
        return {poly};
    case BACK:
        if (idx == polys.size() - 1) {
            return {};
        }

        return ClipToList(polys, poly, clipOnPlane, idx + 1);
    case ONPLANE: {
        return {poly};
        float angle = me->plane.normal.dot(poly.plane.normal) - 1.0;
        if (angle < MapConvert::Epsilon && angle > -MapConvert::Epsilon) {
            if (!clipOnPlane) {
                return {poly};
            }
        }

        if (idx == polys.size() - 1) {
            return {};
        }

        return ClipToList(polys, poly, clipOnPlane, idx + 1);
    }
    case SPLIT: {
        Poly front, back;
        me->SplitPoly(poly, front, back);

        if (idx == polys.size() - 1) {
            return {front};
        }

        auto backFrags = ClipToList(polys, back, clipOnPlane, idx + 1);
        if (backFrags.empty()) {
            return {front};
        }

        if (backFrags.at(0) == back) {
            return {poly};
        }

        backFrags.insert(backFrags.begin(), front);
        return backFrags;
    }
    }

    return {};
}