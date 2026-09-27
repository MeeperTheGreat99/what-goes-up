#pragma once
#include "poly.h"
#include <map>
#include <string>
#include <vector>

struct MapEnt {
    std::map<std::string, std::string> properties;
    std::vector<Poly> polys;
    std::vector<std::vector<size_t>> collisions;
};