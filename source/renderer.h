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

    static constexpr int kShadowResolution = 512;

    static Renderer* Instance;

    Renderer();
    ~Renderer();

    void SetCamera(Camera* camera);
    Camera* GetCamera() { return m_camera; }
    void Draw();
    void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color);
    void DrawText(const char* text, Font* font, int size, int x, int y, glm::vec4 color = glm::vec4(1.0f), TextJustify just = TextJustify());
    void Resize(unsigned int width, unsigned int height);
    void KillLights();
    void SetSubtitleText(std::string text, float duration);
    void SetInfoText(std::string text);
    void SetBlackScreen(bool blackScreen);
    void SetTitleCard(bool titleCard);

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
    unsigned int m_shadowCubemap;
    unsigned int m_shadowFBO;
    bool m_pingPongState;
    LineShader* m_lineShader;
    ObjectShader* m_objectShader;
    SkyShader* m_skyShader;
    ShadowShader* m_shadowShader;
    LightSphereShader* m_lightSphereShader;
    LightShader* m_lightShader;
    Shader* m_screenShader;
    TextShader* m_textShader;
    Model* m_lightSphere;
    Model* m_cube;
    Cubemap* m_reflection;
    Cubemap* m_sky;
    std::vector<Light*> m_lights;
    std::string m_subtitleText;
    float m_subtitleEndTime;
    std::string m_infoText;
    bool m_blackScreen;
    bool m_titleCard;
    Font* m_font;
    Camera* m_camera;
    Audio* m_audio;

    void DrawScene(bool isShadowPass, Camera::Frustum* frustum);
    float XNDC(int x);
    float YNDC(int y);
    void CreateFramebuffers();
    void DeleteFramebuffers();
    void RemakeFramebuffers();
    void TextCharPosition(int x, int y, int w, int h);
    float GetTextWidth(const char* text, Font* font, int size);
};