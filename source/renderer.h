#pragma once
#include "camera.h"
#include "font.h"
#include "shader.h"

struct TextJustify {
    enum class H {
        kLeft,
        kCenter,
        kRight
    };

    enum class V {
        kTop,
        kCenter,
        kBottom
    };

    TextJustify() : horz(H::kLeft), vert(V::kTop) {}
    TextJustify(H horz) : horz(horz), vert(V::kTop) {}
    TextJustify(V vert) : horz(H::kLeft), vert(vert) {}
    TextJustify(H horz, V vert) : horz(horz), vert(vert) {}

    H horz;
    V vert;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    void SetCamera(Camera* camera);
    void Draw();
    void DrawText(const char* text, Font* font, int size, int x, int y, glm::vec4 color = glm::vec4(1.0f), TextJustify just = TextJustify());
    void Resize(unsigned int width, unsigned int height);

private:
    unsigned int m_width, m_height;
    unsigned int m_screenVAO, m_screenVBO;
    unsigned int m_quadVAO, m_quadVBO;
    unsigned int m_gBuffer, m_gBufferDepth;
    unsigned int m_gBufferTextures[3];
    Shader* m_objectShader;
    Shader* m_terrainShader;
    Shader* m_screenShader;
    TextShader* m_textShader;
    Camera* m_camera;
    Font* m_font;

    float XNDC(int x);
    float YNDC(int y);
    void CreateFramebuffers();
    void DeleteFramebuffers();
    void RemakeFramebuffers();
    void TextCharPosition(int x, int y, int w, int h);
};