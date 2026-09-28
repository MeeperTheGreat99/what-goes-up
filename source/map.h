#pragma once
#include <string>

class Map {
public:
    static void Load(std::string name);
    static std::string GetCurrentMap() {return CurrentMap;}

private:
    static std::string CurrentMap;

    static void Unload();
};