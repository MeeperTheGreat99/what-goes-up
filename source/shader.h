#pragma once
#include <glm/mat4x4.hpp>
#include <string>
#include <map>

class Shader {
public:
    struct Constant {
        std::string identifier;
        std::string value;

        Constant() : value("") {}

        Constant(std::string id, int v) : identifier(id) {
            value = std::to_string(v);
        }

        Constant(std::string id, float v) : identifier(id) {
            value = std::to_string(v);
        }
    };

    Shader(std::string vpath, std::string fpath);
    ~Shader();

    void Define(Constant value);
    void Undefine(std::string identifier);
    void Finalize();
    void Reload();
    void Use();

    int GetUniformLocation(std::string uniform);
    void SetUniform(int location, int value);
    void SetUniform(int location, float value);
    void SetUniform(int location, glm::vec2 value);
    void SetUniform(int location, glm::vec3 value);
    void SetUniform(int location, glm::vec4 value);
    void SetUniform(int location, glm::mat4 value);

protected:
    virtual void Load();

private:
    std::string m_vpath, m_fpath;
    std::map<std::string, Constant> m_constants;
    int m_program;

    void Unload();
    int LoadShader(std::string path, int type);
    void CompileShader(int shader);
    void LinkProgram(int program);
};

class LineShader : public Shader {
public:
    LineShader() : Shader("res/shaders/line.vs", "res/shaders/line.fs") {}

    void SetProj(glm::mat4 proj) {
        SetUniform(m_uProj, proj);
    }

    void SetView(glm::mat4 view) {
        SetUniform(m_uView, view);
    }

protected:
    int m_uProj;
    int m_uView;

    virtual void Load() override {
        Shader::Load();
        m_uProj = GetUniformLocation("Proj");
        m_uView = GetUniformLocation("View");
    }
};

class TextShader : public Shader {
public:
    TextShader() : Shader("res/shaders/text.vs", "res/shaders/text.fs") {}

    void SetPosition(glm::vec2 position) {
        SetUniform(m_uPosition, position);
    }

    void SetScale(glm::vec2 scale) {
        SetUniform(m_uScale, scale);
    }

    void SetColor(glm::vec4 color) {
        SetUniform(m_uColor, color);
    }

protected:
    int m_uPosition;
    int m_uScale;
    int m_uColor;

    virtual void Load() override {
        Shader::Load();
        m_uPosition = GetUniformLocation("position");
        m_uScale = GetUniformLocation("scale");
        m_uColor = GetUniformLocation("color");
    }
};

class ObjectShader : public Shader {
public:
    ObjectShader() : Shader("res/shaders/object.vs", "res/shaders/object.fs") {}

    void SetProj(glm::mat4 proj) {
        SetUniform(m_uProj, proj);
    }

    void SetView(glm::mat4 view) {
        SetUniform(m_uView, view);
    }

    void SetModel(glm::mat4 model) {
        SetUniform(m_uModel, model);
    }

    void SetAlbedoTex(int slot) {
        SetUniform(m_uAlbedoSlot, slot);
    }

    void SetAlbedoColor(glm::vec3 color) {
        SetUniform(m_uAlbedoColor, color);
    }

    void SetNormalTex(int slot) {
        SetUniform(m_uNormalSlot, slot);
    }

    void SetNormalPresent(bool present) {
        SetUniform(m_uNormalPresent, (int)present);
    }

    void SetReflectivity(float reflectivity) {
        SetUniform(m_uReflectivity, reflectivity);
    }

protected:
    int m_uProj;
    int m_uView;
    int m_uModel;
    int m_uAlbedoSlot;
    int m_uAlbedoColor;
    int m_uNormalSlot;
    int m_uNormalPresent;
    int m_uReflectivity;

    virtual void Load() override {
        Shader::Load();
        m_uProj = GetUniformLocation("Proj");
        m_uView = GetUniformLocation("View");
        m_uModel = GetUniformLocation("Model");
        m_uAlbedoSlot = GetUniformLocation("tex_albedo");
        m_uAlbedoColor = GetUniformLocation("albedo");
        m_uNormalSlot = GetUniformLocation("tex_normal");
        m_uNormalPresent = GetUniformLocation("tex_normal_present");
        m_uReflectivity = GetUniformLocation("reflectivity");
    }
};

class LightSphereShader : public Shader {
public:
    LightSphereShader() : Shader("res/shaders/object.vs", "res/shaders/lightsphere.fs") {}

    void SetProj(glm::mat4 proj) {
        SetUniform(m_uProj, proj);
    }

    void SetView(glm::mat4 view) {
        SetUniform(m_uView, view);
    }

    void SetModel(glm::mat4 model) {
        SetUniform(m_uModel, model);
    }

    void SetResolution(glm::vec2 resolution) {
        SetUniform(m_uResolution, resolution);
    }

    void SetPositionTex(int slot) {
        SetUniform(m_ugPosition, slot);
    }

    void SetNormalTex(int slot) {
        SetUniform(m_ugNormal, slot);
    }

    void SetLightPosition(glm::vec3 position) {
        SetUniform(m_uLightPosition, position);
    }

    void SetLightColor(glm::vec3 color) {
        SetUniform(m_uLightColor, color);
    }

private:
    int m_uProj;
    int m_uView;
    int m_uModel;
    int m_uResolution;
    int m_ugPosition;
    int m_ugNormal;
    int m_uLightPosition;
    int m_uLightColor;

    virtual void Load() override {
        Shader::Load();
        m_uProj = GetUniformLocation("Proj");
        m_uView = GetUniformLocation("View");
        m_uModel = GetUniformLocation("Model");
        m_uResolution = GetUniformLocation("resolution");
        m_ugPosition = GetUniformLocation("gPosition");
        m_ugNormal = GetUniformLocation("gNormal");
        m_uLightPosition = GetUniformLocation("light_position");
        m_uLightColor = GetUniformLocation("light_color");
    }
};

class LightShader : public Shader {
public:
    LightShader() : Shader("res/shaders/screen.vs", "res/shaders/light.fs") {}

    void SetViewPos(glm::vec3 pos) {
        SetUniform(m_uViewPos, pos);
    }

    void SetPositionTex(int slot) {
        SetUniform(m_ugPosition, slot);
    }

    void SetNormalTex(int slot) {
        SetUniform(m_ugNormal, slot);
    }

    void SetAlbedoSpecTex(int slot) {
        SetUniform(m_ugAlbedoSpec, slot);
    }

    void SetLightTex(int slot) {
        SetUniform(m_ugLight, slot);
    }

    void SetReflectivityTex(int slot) {
        SetUniform(m_ugReflectivity, slot);
    }

    void SetReflectionTex(int slot) {
        SetUniform(m_uReflectionSlot, slot);
    }

protected:
    int m_uViewPos;
    int m_ugPosition;
    int m_ugNormal;
    int m_ugAlbedoSpec;
    int m_ugLight;
    int m_ugReflectivity;
    int m_uReflectionSlot;

    virtual void Load() override {
        Shader::Load();
        m_uViewPos = GetUniformLocation("ViewPos");
        m_ugPosition = GetUniformLocation("gPosition");
        m_ugNormal = GetUniformLocation("gNormal");
        m_ugAlbedoSpec = GetUniformLocation("gAlbedoSpec");
        m_ugLight = GetUniformLocation("gLight");
        m_ugReflectivity = GetUniformLocation("gReflectivity");
        m_uReflectionSlot = GetUniformLocation("tex_reflection");
    }
};