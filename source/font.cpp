#include "font.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <msdfgen.h>
#include <msdfgen-ext.h>

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

    msdfgen::FontHandle* hFont = msdfgen::adoptFreetypeFont(face);

    struct GenGlyph {
        Glyph* glyph;
        msdfgen::Shape shape;
        msdfgen::SDFTransformation transform;
    };
    
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
        msdfgen::Shape::Bounds bounds = shape.getBounds();
        double aspect = fabs((bounds.r - bounds.l) / (bounds.t - bounds.b));

        msdfgen::edgeColoringSimple(shape, 3.0);
        msdfgen::Bitmap<float, 3> msdf(fontSize, fontSize / aspect);

        msdfgen::Vector2 scale(msdf.width(), msdf.height() * aspect);
        msdfgen::Vector2 translate(0.0, double(msdf.height() - msdf.width()) / msdf.width());
        msdfgen::SDFTransformation transform(msdfgen::Projection(scale, translate), msdfgen::Range(range / fontSize));
        
        msdfgen::generateMSDF(msdf, shape, transform);

        unsigned char* image = new unsigned char[msdf.width() * msdf.height() * 3];
        for (int y = 0; y < msdf.height(); y++) {
            for (int x = 0; x < msdf.width(); x++) {
                int idx = (y * msdf.width() + x) * 3;
                image[idx] = msdfgen::pixelFloatToByte(msdf(x, y)[0]);
                image[idx+1] = msdfgen::pixelFloatToByte(msdf(x, y)[1]);
                image[idx+2] = msdfgen::pixelFloatToByte(msdf(x, y)[2]);
            }
        }

        glyph.texture = new Texture(msdf.width(), msdf.height(), 3, image, false);
        glyph.width = msdf.width();
        glyph.height = msdf.height();
        delete[] image;

        glyphs[ch] = glyph;
    } while((ch = FT_Get_Next_Char(face, ch, &glyphIdx)));

    msdfgen::destroyFont(hFont);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return new Font(fontSize, fontHeight, glyphs);
}

Font::Font(int size, int height, const GlyphMap& glyphs) :
m_size(size), m_height(height), m_glyphs(glyphs) {}

Font::~Font() {
    for (auto& entry : m_glyphs) {
        delete entry.second.texture;
    }
}