#pragma once
#include "vector.h"
#include <string>

class Map {
public:
    static std::string QueuedMap;

    static void Load(std::string name);
    static std::string GetCurrentMap() { return CurrentMap; }
    static Vector GetPlayerPosition() { return PlayerPosition; }
    static Vector GetPlayerAngles() { return PlayerAngles; }

private:
    static std::string CurrentMap;
    static Vector PlayerPosition;
    static Vector PlayerAngles;

    static void Unload();
};