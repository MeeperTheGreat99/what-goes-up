#include "texture.h"
#include "gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture* Texture::DefaultTexture = nullptr;
Texture* Texture::WhiteTexture = nullptr;

Texture* Texture::GetDefault() {
    if (!DefaultTexture) {
        unsigned char* image = new unsigned char[16*16*3];
        memset(image, 0, 16*16*3);

        bool flip = false;
        for (int y = 0; y < 16; y++) {
            bool flop = true;
            for (int x = 0; x < 16; x++) {
                int idx = (y * 16 + x) * 3;
                if (flip ^ flop) {
                    image[idx] = 255;
                    image[idx+2] = 255;
                }
                flop = !flop;
            }
            flip = !flip;
        }

        DefaultTexture = new Texture(16, 16, 3, image, false, true);
        delete[] image;
    }

    return DefaultTexture;
}

Texture* Texture::GetWhite() {
    if (!WhiteTexture) {
        unsigned char* image = new unsigned char[2*2*3];
        memset(image, 255, 2*2*3);
        WhiteTexture = new Texture(2, 2, 3, image, false, false);
        delete[] image;
    }

    return WhiteTexture;
}

Texture* Texture::Load(std::string filename, bool srgb, bool repeat) {
    int width, height, chan;
    unsigned char* image = stbi_load(filename.c_str(), &width, &height, &chan, 0);
    if (!image) {
        return nullptr;
    }

    Texture* texture = new Texture(width, height, chan, image, srgb, repeat);
    stbi_image_free(image);
    
    return texture;
}

Texture* Texture::LoadMemory(unsigned char* data, size_t size, bool srgb, bool repeat) {
    int width, height, chan;
    unsigned char* image = stbi_load_from_memory(data, size, &width, &height, &chan, 0);
    if (!image) {
        return nullptr;
    }

    Texture* texture = new Texture(width, height, chan, image, srgb, repeat);
    stbi_image_free(image);
    
    return texture;
}

Texture::Texture(int width, int height, int chan, unsigned char* image, bool srgb, bool repeat) :
m_width(width), m_height(height) {
    srgb = false;
    int format, iformat;
    if (srgb) {
        switch (chan) {
        case 3:
            format = GL_SRGB;
            iformat = GL_SRGB8;
            break;
        case 4:
        default:
            format = GL_SRGB_ALPHA;
            iformat = GL_SRGB8_ALPHA8;
            break;
        }
    } else {
        switch (chan) {
        case 1:
            format = GL_RED;
            iformat = GL_R8;
            break;
        case 2:
            format = GL_RG;
            iformat = GL_RG8;
            break;
        case 3:
        default:
            format = GL_RGB;
            iformat = GL_RGB8;
            break;
        case 4:
            format = GL_RGBA;
            iformat = GL_RGBA8;
            break;
        }
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, iformat,
        width, height, 0,
        format, GL_UNSIGNED_BYTE, image
    );
    if (repeat) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

Texture::~Texture() {
    glDeleteTextures(1, &m_texture);
}

void Texture::Use(int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}