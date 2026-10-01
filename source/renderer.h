#pragma once
#include "audio.h"
#include "camera.h"
#include "cubemap.h"
#include "font.h"
#include "light.h"
#include "model.h"
#include "shader.h"
#include <vector>

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
    friend class Light;

    static Renderer* Instance;

    Renderer();
    ~Renderer();

    void SetCamera(Camera* camera);
    void Draw();
    void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color);
    void DrawText(const char* text, Font* font, int size, int x, int y, glm::vec4 color = glm::vec4(1.0f), TextJustify just = TextJustify());
    void Resize(unsigned int width, unsigned int height);
    void KillLights();

    void SetAudio(Audio* audio) {m_audio = audio;}

private:
    unsigned int m_width, m_height;
    unsigned int m_screenVAO, m_screenVBO;
    unsigned int m_quadVAO, m_quadVBO;
    unsigned int m_lineVAO, m_lineVBO;
    unsigned int m_gBuffer, m_gBufferDepth;
    unsigned int m_gBufferTextures[4];
    unsigned int m_pingPongFBO[2];
    unsigned int m_pingPongTextures[2];
    unsigned int m_pingPongDepth[2];
    bool m_pingPongState;
    LineShader* m_lineShader;
    ObjectShader* m_objectShader;
    LightSphereShader* m_lightSphereShader;
    LightShader* m_lightShader;
    Shader* m_screenShader;
    TextShader* m_textShader;
    Model* m_lightSphere;
    Cubemap* m_reflection;
    std::vector<Light*> m_lights;
    Font* m_font;
    Camera* m_camera;
    Audio* m_audio;

    float XNDC(int x);
    float YNDC(int y);
    void CreateFramebuffers();
    void DeleteFramebuffers();
    void RemakeFramebuffers();
    void TextCharPosition(int x, int y, int w, int h);
};