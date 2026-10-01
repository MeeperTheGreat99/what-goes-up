#include "font.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <functional>
#include <msdfgen.h>
#include <msdfgen-ext.h>
#include <thread>

struct GenGlyph {
    int fontSize;
    double range;
    unsigned char* image;
    Font::Glyph* glyph;
    msdfgen::Shape shape;
};

void GenerateThread(std::vector<GenGlyph>& glyphs) {
    for (GenGlyph& glyph : glyphs) {
        msdfgen::Shape::Bounds bounds = glyph.shape.getBounds();
        double aspect = fabs((bounds.r - bounds.l) / (bounds.t - bounds.b));

        msdfgen::edgeColoringSimple(glyph.shape, 3.0);
        msdfgen::Bitmap<float, 3> msdf(glyph.fontSize, glyph.fontSize / aspect);

        msdfgen::Vector2 scale(msdf.width(), msdf.height() * aspect);
        msdfgen::Vector2 translate(0.0, double(msdf.height() - msdf.width()) / msdf.width());
        msdfgen::SDFTransformation transform(msdfgen::Projection(scale, translate), msdfgen::Range(glyph.range / glyph.fontSize));

        msdfgen::generateMSDF(msdf, glyph.shape, transform);
        glyph.glyph->width = msdf.width();
        glyph.glyph->height = msdf.height();
        glyph.image = new unsigned char[msdf.width() * msdf.height() * 3];
        for (int y = 0; y < msdf.height(); y++) {
            for (int x = 0; x < msdf.width(); x++) {
                int idx = (y * msdf.width() + x) * 3;
                glyph.image[idx] = msdfgen::pixelFloatToByte(msdf(x, y)[0]);
                glyph.image[idx+1] = msdfgen::pixelFloatToByte(msdf(x, y)[1]);
                glyph.image[idx+2] = msdfgen::pixelFloatToByte(msdf(x, y)[2]);
            }
        }
    }
}

Font* Font::Load(std::string filename) {
    FT_Library ft;
    FT_Face face;

    if (FT_Init_FreeType(&ft) != FT_Err_Ok) {
        return nullptr;
    }

    if (FT_New_Face(ft, filename.c_str(), 0, &face) != FT_Err_Ok) {
        FT_Done_FreeType(ft);
        return nullptr;
    }

    int fontSize = 32;
    double range = 4.0;
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    int fontHeight = face->height >> 6;
    int ascension = face->size->metrics.ascender >> 6;

    std::vector<GenGlyph> genGlyphs;

    msdfgen::FontHandle* hFont = msdfgen::adoptFreetypeFont(face);
    
    GlyphMap glyphs;
    uint32_t glyphIdx;
    uint32_t ch = FT_Get_First_Char(face, &glyphIdx);
    do {
        FT_Load_Glyph(face, glyphIdx, FT_LOAD_NO_BITMAP);

        Glyph glyph;
        glyph.width = fontSize;
        glyph.height = fontSize;
        glyph.ofsX = face->glyph->bitmap_left;
        glyph.ofsY = -face->glyph->bitmap_top + ascension;
        glyph.advance = face->glyph->advance.x >> 6;

        msdfgen::Shape shape;
        msdfgen::loadGlyph(shape, hFont, msdfgen::GlyphIndex(glyphIdx), msdfgen::FONT_SCALING_EM_NORMALIZED);
        shape.orientContours();
        shape.normalize();
        
        glyphs[ch] = glyph;

        GenGlyph genGlyph;
        genGlyph.fontSize = fontSize;
        genGlyph.range = range;
        genGlyph.glyph = &glyphs[ch];
        genGlyph.shape = shape;
        genGlyphs.push_back(genGlyph);
    } while((ch = FT_Get_Next_Char(face, ch, &glyphIdx)));

    msdfgen::destroyFont(hFont);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    int threadCount = 8;
    size_t perThread = genGlyphs.size() / threadCount;
    std::vector<std::thread> threads;
    std::vector<std::vector<GenGlyph>> chunks(threadCount);

    for (int i = 0; i < threadCount; i++) {
        int count = std::min(perThread, genGlyphs.size());
        if (i == threadCount - 1) {
            count = genGlyphs.size();
        }

        std::vector<GenGlyph>& chunk = chunks[i];
        chunk.insert(chunk.begin(), genGlyphs.begin(), genGlyphs.begin() + count);
        genGlyphs.erase(genGlyphs.begin(), genGlyphs.begin() + count);
        threads.emplace_back(GenerateThread, std::ref(chunk));
    }

    for (int i = 0; i < threadCount; i++) {
        threads[i].join();
    }

    for (auto& chunk : chunks) {
        for (GenGlyph& genGlyph : chunk) {
            genGlyph.glyph->texture = new Texture(genGlyph.glyph->width, genGlyph.glyph->height, 3, genGlyph.image, false);
            delete[] genGlyph.image;
        }
    }

    return new Font(fontSize, fontHeight, glyphs);
}

Font::Font(int size, int height, const GlyphMap& glyphs) :
m_size(size), m_height(height), m_glyphs(glyphs) {}

Font::~Font() {
    for (auto& entry : m_glyphs) {
        delete entry.second.texture;
    }
}