#include "renderer.h"
#include "entity.h"
#include "gl.h"
#include "shader.h"
#include "texture.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

Renderer* Renderer::Instance = nullptr;

static float screenVertices[] = {
    -1.0f, 3.0f, 0.0f, 2.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    3.0f, -1.0f, 2.0f, 0.0f,
};

static float quadVertices[] = {
    -1.0f, 1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
};

static float lineVertices[] = {
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
};

Renderer::Renderer() {
    m_width = 800;
    m_height = 600;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glGenVertexArrays(1, &m_screenVAO);
    glBindVertexArray(m_screenVAO);
    glGenBuffers(1, &m_screenVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

    glGenVertexArrays(1, &m_quadVAO);
    glBindVertexArray(m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

    glGenVertexArrays(1, &m_lineVAO);
    glBindVertexArray(m_lineVAO);
    glGenBuffers(1, &m_lineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

    CreateFramebuffers();

    m_lineShader = new LineShader();
    m_lineShader->Finalize();

    m_objectShader = new ObjectShader();
    m_objectShader->Finalize();
    m_objectShader->SetAlbedoTex(0);

    m_lightSphereShader = new LightSphereShader();
    m_lightSphereShader->Finalize();
    m_lightSphereShader->SetPositionTex(0);
    m_lightSphereShader->SetNormalTex(1);

    m_lightShader = new LightShader();
    m_lightShader->Finalize();
    m_lightShader->SetPositionTex(0);
    m_lightShader->SetNormalTex(1);
    m_lightShader->SetAlbedoSpecTex(2);
    m_lightShader->SetLightTex(3);
    
    m_screenShader = new Shader("res/shaders/screen.vs", "res/shaders/screen.fs");
    m_screenShader->Finalize();
    m_screenShader->SetUniform(m_screenShader->GetUniformLocation("textie"), 0);

    m_textShader = new TextShader();
    m_textShader->Finalize();

    m_lightSphere = Model::LoadExternal("res/models/light.obj");
    
    m_font = Font::Load("res/fonts/raleway.ttf");
    
    m_camera = nullptr;

    Instance = this;
}

Renderer::~Renderer() {
    delete m_font;
    delete m_lightSphere;
    delete m_textShader;
    delete m_screenShader;
    delete m_lightShader;
    delete m_lightSphereShader;
    delete m_objectShader;
    delete m_lineShader;

    glBindVertexArray(0);
    glDeleteBuffers(1, &m_quadVBO);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_screenVBO);
    glDeleteVertexArrays(1, &m_screenVAO);

    DeleteFramebuffers();
}

void Renderer::SetCamera(Camera* camera) {
    m_camera = camera;
    if (m_camera) {
        m_camera->SetAspect((float)m_width / m_height);
    }
}

void Renderer::Draw() {
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_objectShader->Use();

    if (m_camera) {
        m_objectShader->SetProj(m_camera->GetProj());
        m_objectShader->SetView(m_camera->GetView());
        m_objectShader->SetUniform(m_objectShader->GetUniformLocation("View"), m_camera->GetView());
        m_audio->SetListenerPos(m_camera->GetPos());
        m_audio->SetListenerDir(m_camera->GetAng().direction());
    }

    for (auto& entry : Entity::Entities) {
        Entity* entity = entry.second;
        if (entity->IsSpawned()) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), entity->GetPos().gl());
            model *= glm::mat4_cast(entity->GetRot().gl());
            m_objectShader->SetModel(model);
            entity->Draw(m_objectShader);
        }
    }

    // BEGIN DEFERRED LIGHTING PASS

    glDisable(GL_DEPTH_TEST);

    m_pingPongState = false;
    glBindFramebuffer(GL_FRAMEBUFFER, m_pingPongFBO[m_pingPongState]);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glCullFace(GL_FRONT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gBufferTextures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gBufferTextures[1]);

    m_lightSphereShader->Use();
    m_lightSphereShader->SetResolution(glm::vec2((float)m_width, (float)m_height));
    if (m_camera) {
        m_lightSphereShader->SetProj(m_camera->GetProj());
        m_lightSphereShader->SetView(m_camera->GetView());
    }

    for (Light* light : m_lights) {
        float radius = sqrtf(light->GetIntensity() * 1024.0f);
        glm::mat4 model(1.0f);
        model = glm::translate(model, light->GetPos().gl());
        model = glm::scale(model, glm::vec3(radius));
        m_lightSphereShader->SetModel(model);
        m_lightSphereShader->SetLightPosition(light->GetPos().gl());
        m_lightSphereShader->SetLightColor(light->GetColor().gl() * light->GetIntensity());
        m_lightSphere->Draw();
    }

    glCullFace(GL_BACK);
    glDisable(GL_BLEND);
    m_pingPongState = !m_pingPongState;

    // END DEFERRED LIGHTING PASS

    // BEGIN LIGHTING PASS
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_pingPongFBO[m_pingPongState]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gBufferTextures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gBufferTextures[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gBufferTextures[2]);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_pingPongTextures[!m_pingPongState]);

    m_lightShader->Use();

    glBindVertexArray(m_screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // END LIGHTING PASS

    m_screenShader->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pingPongTextures[m_pingPongState]);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // DrawText("stupid text", nullptr, 16, 0, 0);
}

void Renderer::DrawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color) {
    glBindVertexArray(m_lineVAO);
    memcpy(&lineVertices[0], &start, sizeof(glm::vec3));
    memcpy(&lineVertices[3], &color, sizeof(glm::vec3));
    memcpy(&lineVertices[6], &end, sizeof(glm::vec3));
    memcpy(&lineVertices[9], &color, sizeof(glm::vec3));
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lineVertices), lineVertices);
    
    m_lineShader->Use();
    m_lineShader->SetProj(m_camera ? m_camera->GetProj() : glm::mat4(1.0f));
    m_lineShader->SetView(m_camera ? m_camera->GetView() : glm::mat4(1.0f));

    glDrawArrays(GL_LINES, 0, 2);
}

void Renderer::DrawText(const char* text, Font* font, int size, int x, int y, glm::vec4 color, TextJustify just) {
    m_textShader->Use();
    m_textShader->SetColor(color);
    glBindVertexArray(m_quadVAO);

    if (!font) {
        font = m_font;
    }

    float sizeScalar = (float)size / font->GetSize();
    const Font::GlyphMap& glyphs = font->GetGlyphs();

    int cx = x;
    int i = 0;
    char ch;
    while ((ch = text[i++])) {
        if (!glyphs.count(ch)) {
            continue;
        }

        const Font::Glyph& glyph = glyphs.at(ch);
        if (glyph.texture) {    
            TextCharPosition(x, y, glyph.width * sizeScalar, glyph.height * sizeScalar);
            glyph.texture->Use(0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        x += glyph.advance * sizeScalar;
    }
}

void Renderer::Resize(unsigned int width, unsigned int height) {
    m_width = width;
    m_height = height;
    if (m_camera) {
        m_camera->SetAspect((float)width / height);
    }
    RemakeFramebuffers();
}

float Renderer::XNDC(int x) {
    return ((float)x / m_width) * 2.0f - 1.0f;
}

float Renderer::YNDC(int y) {
    return ((float)y / m_height) * -2.0f + 1.0f;
}

void Renderer::CreateFramebuffers() {
    glGenFramebuffers(1, &m_gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

    glGenTextures(1, &m_gBufferTextures[0]);
    glBindTexture(GL_TEXTURE_2D, m_gBufferTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBufferTextures[0], 0);

    glGenTextures(1, &m_gBufferTextures[1]);
    glBindTexture(GL_TEXTURE_2D, m_gBufferTextures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBufferTextures[1], 0);

    glGenTextures(1, &m_gBufferTextures[2]);
    glBindTexture(GL_TEXTURE_2D, m_gBufferTextures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gBufferTextures[2], 0);

    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(sizeof(buffers) / sizeof(buffers[0]), buffers);

    glGenRenderbuffers(1, &m_gBufferDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_gBufferDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gBufferDepth);

    glGenFramebuffers(2, m_pingPongFBO);
    glGenTextures(2, m_pingPongTextures);
    glGenRenderbuffers(2, m_pingPongDepth);
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingPongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_pingPongTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingPongTextures[i], 0);

        glBindRenderbuffer(GL_RENDERBUFFER, m_pingPongDepth[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_pingPongDepth[i]);
    }
}

void Renderer::DeleteFramebuffers() {
    glDeleteRenderbuffers(2, m_pingPongDepth);
    glDeleteTextures(2, m_pingPongTextures);
    glDeleteFramebuffers(2, m_pingPongFBO);

    glDeleteRenderbuffers(1, &m_gBufferDepth);
    for (int i = 0; i < sizeof(m_gBufferTextures) / sizeof(m_gBufferTextures[0]); i++) {
        glDeleteTextures(1, &m_gBufferTextures[i]);
    }
    glDeleteFramebuffers(1, &m_gBuffer);
}

void Renderer::RemakeFramebuffers() {
    DeleteFramebuffers();
    CreateFramebuffers();
}

void Renderer::TextCharPosition(int x, int y, int w, int h) {
    glm::vec2 position(XNDC(x), YNDC(y));
    glm::vec2 scale((float)w / m_width, (float)h / m_height);
    m_textShader->SetPosition(position);
    m_textShader->SetScale(scale);
}