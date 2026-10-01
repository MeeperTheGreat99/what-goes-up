#pragma once
#include <string>

class Cubemap {
public:
    Cubemap(std::string path);
    ~Cubemap();

    void Use(int slot);

private:
    unsigned int m_id;
};