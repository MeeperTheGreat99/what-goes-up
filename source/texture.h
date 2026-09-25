#pragma once
#include "gl.h"
#include <string>

class Texture {
public:
    static Texture* GetDefault();
    static Texture* Load(std::string filename, bool srgb, bool repeat = false);
    static Texture* LoadMemory(unsigned char* data, size_t size, bool srgb, bool repeat = false);

    Texture(int width, int height, int chan, unsigned char* image, bool srgb, bool repeat = false);
    ~Texture();

    void Use(int slot);

    int GetWidth() const {return m_width;}
    int GetHeight() const {return m_height;}

private:
    static Texture* DefaultTexture;
    
    int m_width, m_height;
    unsigned int m_texture;
};