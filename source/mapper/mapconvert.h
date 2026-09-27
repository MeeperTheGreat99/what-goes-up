#pragma once
#include "plane.h"
#include <sstream>
#include <vector>

class Brush;
class MapEnt;

class MapConvert {
public:
    struct Vertex {
        Vector point;
        double uv[2];
        size_t ID;
    };

    struct Face {
        Plane plane;
        Plane axes[2];
        double scales[2];
        struct {
            std::string name;
            int width, height;
        } texture;
    };

    static constexpr double Epsilon = 0.00001;
    static constexpr double BigEpsilon = 0.01;
    
    static size_t VertexID;

    MapConvert(char* data, size_t size);
    ~MapConvert();

    bool Convert();
    std::vector<MapEnt*> GetEntities();

private:
    std::string m_data;
    std::istringstream m_stream;
    std::vector<MapEnt*> m_entities;
    std::string m_token;
    size_t m_tokenStart;
    size_t m_tokenEnd;

    bool IsStreamBad() const;
    bool GetToken(bool advance = true);
    bool GetToken(bool advance, size_t origin);
    bool ParseString(std::string& str);
    bool ParseVector(Vector& vec);
    bool ParsePlane(Plane& plane);
    bool ParseEntity();
    bool ParseBrush(Brush& brush);
    bool ParseFace(Face& face);
};