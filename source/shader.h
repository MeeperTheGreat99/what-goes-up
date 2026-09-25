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
    void SetUniform(int location, glm::vec2 value);
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