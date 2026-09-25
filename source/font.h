#pragma once
#include "texture.h"
#include <unordered_map>

class Font {
public:
    struct Glyph {
        int ofsX, ofsY;
        int width, height;
        int advance;
        Texture* texture;
    };

    typedef std::unordered_map<uint32_t, Glyph> GlyphMap;

    static Font* Load(std::string filename);

    Font(int size, int height, const GlyphMap& glyphs);
    ~Font();

    int GetSize() const {return m_size;}
    int GetHeight() const {return m_height;}
    const GlyphMap& GetGlyphs() const {return m_glyphs;}

private:
    int m_size, m_height;
    std::unordered_map<uint32_t, Glyph> m_glyphs;
};