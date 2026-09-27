#include "mapconvert.h"
#include "brush.h"
#include "../texture.h"
#include "mapent.h"
#include "poly.h"

using namespace std;

size_t MapConvert::VertexID = 1;

MapConvert::MapConvert(char* data, size_t size) {
    m_data = string(data, size);
    m_stream = istringstream(m_data);
    m_entities = {};
}

MapConvert::~MapConvert() {
    for (auto ent : m_entities) {
        delete ent;
    }
    m_entities.clear();
}

bool MapConvert::Convert() {
    VertexID = 1;
    if (!GetToken(false)) {
        return false;
    }

    while (true) {
        if (!GetToken(false)) {
            break;
        }

        if (m_token != "{") {
            printf("Expected \"{\", got \"%s\"\n", m_token.c_str());
            return false;
        }
        
        if (!ParseEntity()) {
            printf("Failed to parse entity\n");
            return false;
        }
    }

    return true;
}

std::vector<MapEnt*> MapConvert::GetEntities() {
    return m_entities;
}

bool MapConvert::IsStreamBad() const {
    return !m_stream.good() || m_stream.fail() || m_stream.eof();
}

bool MapConvert::GetToken(bool advance) {
    return GetToken(advance, -1);
}

bool MapConvert::GetToken(bool advance, size_t origin) {
    if (origin == -1) {
        origin = m_stream.tellg();
    }

    m_token = "";

    // try to get a char
    char ch = m_stream.get();
    if (IsStreamBad()) {
        m_stream.clear();
        if (!advance) {
            m_stream.seekg(origin, std::ios::beg);
        }

        return false;
    }

    // skip whitespace
    while (std::isspace(ch)) {
        ch = m_stream.get();
        if (IsStreamBad()) {
            m_stream.clear();
            if (!advance) {
                m_stream.seekg(origin, std::ios::beg);
            }

            return false;
        }
    }

    // skip comments
    if (ch == '/') {
        ch = m_stream.get();
        if (!IsStreamBad() && ch == '/') {
            while (ch != '\n' && !IsStreamBad()) {
                ch = m_stream.get();
            }

            if (IsStreamBad()) {
                m_stream.clear();
                if (!advance) {
                    m_stream.seekg(origin, std::ios::beg);
                }

                return false;
            }

            return GetToken(advance, origin);
        } else {
            if (IsStreamBad()) {
                m_stream.clear();
            }
            
            m_stream.seekg(-1, std::ios::cur);
            ch = '/';
        }
    }

    m_tokenStart = m_stream.tellg();
    m_tokenStart--;

    int i = 0;
    while (!std::isspace(ch) && !IsStreamBad()) {
        m_token += ch;
        ch = m_stream.get();
        i++;
    }

    m_tokenEnd = m_stream.tellg();

    if (IsStreamBad()) {
        m_stream.clear();
    }

    if (!advance) {
        m_stream.seekg(origin, std::ios::beg);
    }

    return true;
}

bool MapConvert::ParseString(std::string& str) {
    char ch = m_stream.get();
    if (ch != '"') {
        return false;
    }
    
    str = "";
    do {
        ch = m_stream.get();
        if (IsStreamBad()) {
            return false;
        } else if (ch != '"') {
            str += ch;
        }
    } while (ch != '"');

    return true;
}

bool MapConvert::ParseVector(Vector& vec) {
    if (!GetToken() || m_token != "(") {
        return false;
    }

    if (!GetToken()) {
        return false;
    }

    vec.x = std::stod(m_token);

    if (!GetToken()) {
        return false;
    }

    vec.y = std::stod(m_token);

    if (!GetToken()) {
        return false;
    }

    vec.z = std::stod(m_token);

    if (!GetToken() || m_token != ")") {
        return false;
    }

    return true;
}

bool MapConvert::ParsePlane(Plane& plane) {
    if (!GetToken() || m_token != "[") {
        return false;
    }

    if (!GetToken()) {
        return false;
    }

    plane.normal.x = std::stod(m_token);

    if (!GetToken()) {
        return false;
    }

    plane.normal.y = std::stod(m_token);

    if (!GetToken()) {
        return false;
    }

    plane.normal.z = std::stod(m_token);

    if (!GetToken()) {
        return false;
    }

    plane.dist = std::stod(m_token);

    if (!GetToken() || m_token != "]") {
        return false;
    }

    return true;
}

bool MapConvert::ParseEntity() {
    if (!GetToken() || m_token[0] != '{') {
        return false;
    }

    if (!GetToken()) {
        return false;
    }

    std::vector<std::pair<std::string, std::string>> properties = {};
    std::vector<Brush> brushes = {};
    std::string property = "";
    bool inProp = false;

    while (true) {
        if (m_token == "}") {
            break;
        } else if (m_token == "{") {
            m_stream.seekg(m_tokenStart, std::ios::beg);
            Brush brush;
            if (!ParseBrush(brush)) {
                printf("Failed to parse brush\n");
                return false;
            }
            brushes.push_back(brush);
            GetToken();
        } else if (m_token.at(0) == '"') {
            m_stream.seekg(m_tokenStart, std::ios::beg);
            if (!inProp) {
                if (!ParseString(property)) {
                    printf("Failed to parse property name\n");
                    return false;
                }
                inProp = true;
            } else {
                std::string value;
                if (!ParseString(value)) {
                    printf("Failed to parse property value\n");
                    return false;
                }
                inProp = false;
                properties.emplace_back(property, value);
            }
            GetToken();
        } else {
            printf("Unexpected token: \"%s\"\n", m_token.c_str());
            return false;
        }
    }

    MapEnt ent;
    for (auto entry : properties) {
        ent.properties[entry.first] = entry.second;
    }
    auto polys = Brush::MergeList(brushes);
    for (auto& poly : polys) {
        auto triangulated = poly.Triangulate();
        for (auto& tri : triangulated) {
            double tmp = tri.plane.normal.y;
            tri.plane.normal.y = tri.plane.normal.z;
            tri.plane.normal.z = -tmp;
            for (auto& v : tri.vertices) {
                tmp = v.point.y;
                v.point.y = v.point.z;
                v.point.z = -tmp;
                v.point /= 32.0;
            }
            ent.polys.push_back(tri);
        }
    }
    for (auto& brush : brushes) {
        ent.collisions.push_back(brush.collision);
    }
    m_entities.push_back(new MapEnt(ent));

    return true;
}

bool MapConvert::ParseBrush(Brush& brush) {
    if (!GetToken() || m_token != "{") {
        return false;
    }

    if (!GetToken()) {
        return false;
    }

    std::vector<Face> faces = {};
    
    while (true) {
        if (m_token == "}") {
            break;
        } else if (m_token == "(") {
            Face face;
            m_stream.seekg(m_tokenStart, std::ios::beg);
            if (!ParseFace(face)) {
                printf("Failed to parse face\n");
                return false;
            }
            faces.push_back(face);
            GetToken();
        } else {
            printf("Unexpected token: \"%s\"\n", m_token.c_str());
            return false;
        }
    }

    Vector interior;
    if (!Poly::GetInteriorPoint(faces, interior)) {
        printf("fucking piece of garbage\n");
        return false;
    }

    for (auto& face : faces) {
        if (face.plane.Classify(interior) == Plane::FRONT) {
            face.plane.normal *= -1;
            face.plane.dist *= -1;
        }
    }

    std::vector<Poly> polys = Poly::GetPolysFromFaces(faces);
    for (int i = 0; i < faces.size(); i++) {
        Face& face = faces.at(i);
        Poly& poly = polys.at(i);

        poly.plane = face.plane;
        poly.texture = face.texture.name;
        poly.SortVerticesCCW();
        poly.CalculateUVs(
            face.texture.width,
            face.texture.height,
            face.axes, face.scales
        );
    }

    brush = Brush();
    brush.polys = polys;
    brush.CalculateAABB();
    for (Poly& poly : polys) {
        for (Vertex& v : poly.vertices) {
            brush.collision.push_back(v.ID);
        }
    }

    return true;
}

bool MapConvert::ParseFace(Face& face) {
    Vector points[3];

    for (int i = 0; i < 3; i++) {   
        if (!ParseVector(points[i])) {
            printf("Failed to parse vector\n");
            return false;
        }
    }

    face.plane = Plane(points[0], points[1], points[2]);

    if (!GetToken()) {
        printf("Failed to read texture name\n");
        return false;
    }

    face.texture.name = m_token;
    Texture* texture = Texture::Load("res/textures/" + face.texture.name + ".png", false);
    if (texture) {
        face.texture.width = texture->GetWidth();
        face.texture.height = texture->GetHeight();
        if (texture != Texture::GetDefault()) {
            delete texture;
        }
    } else {
        printf("Failed to get texture: %s\n", face.texture.name.c_str());
        return false;
    }

    for (int i = 0; i < 2; i++) {   
        if (!ParsePlane(face.axes[i])) {
            printf("Failed to parse texture axis\n");
            return false;
        }
    }

    if (!GetToken()) {
        printf("Failed to read texture rotation\n");
        return false;
    }

    if (!GetToken()) {
        printf("Failed to read U scale\n");
        return false;
    }

    face.scales[0] = std::stod(m_token);

    if (!GetToken()) {
        printf("Failed to read V scale\n");
        return false;
    }

    face.scales[1] = std::stod(m_token);

    return true;
}