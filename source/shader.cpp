#include "shader.h"
#include "gl.h"
#include <exception>
#include <fstream>
#include <sstream>
#include <stdexcept>

Shader::Shader(std::string vpath, std::string fpath) : m_vpath(vpath), m_fpath(fpath), m_program(0) {}

Shader::~Shader() {
    Unload();
}

void Shader::Define(Shader::Constant value) {
    m_constants[value.identifier] = value;
}

void Shader::Undefine(std::string identifier) {
    if (m_constants.count(identifier)) {
        m_constants.erase(identifier);
    }
}

void Shader::Finalize() {
    Reload();
}

void Shader::Reload() {
    Unload();
    Load();
}

void Shader::Use() {
    glUseProgram(m_program);
}

int Shader::GetUniformLocation(std::string uniform) {
    return glGetUniformLocation(m_program, uniform.c_str());
}

void Shader::SetUniform(int location, int value) {
    glUniform1i(location, value);
}

void Shader::SetUniform(int location, glm::vec2 value) {
    glUniform2fv(location, 1, &value[0]);
}

void Shader::SetUniform(int location, glm::vec3 value) {
    glUniform3fv(location, 1, &value[0]);
}

void Shader::SetUniform(int location, glm::vec4 value) {
    glUniform4fv(location, 1, &value[0]);
}

void Shader::SetUniform(int location, glm::mat4 value) {
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::Load() {
    int vert = LoadShader(m_vpath, GL_VERTEX_SHADER);
    int frag = LoadShader(m_fpath, GL_FRAGMENT_SHADER);
    m_program = glCreateProgram();

    glAttachShader(m_program, vert);
    glAttachShader(m_program, frag);
    LinkProgram(m_program);
    glDeleteShader(vert);
    glDeleteShader(frag);
    Use();
}

void Shader::Unload() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
}

int Shader::LoadShader(std::string path, int type) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return 0;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* source = new char[size];
    file.read(source, size);
    file.close();

    std::string sourceStr(source, size);
    delete[] source;

    std::istringstream stream(sourceStr);
    std::string version, data, line;

    std::getline(stream, version);
    data += version + '\n';

    // define all constants
    for (auto& entry : m_constants) {
        if (entry.second.value.empty()) {
            continue;
        }

        data += "#define " + entry.second.identifier + ' ' + entry.second.value + '\n';
    }

    // append the remaining shader code
    while (std::getline(stream, line)) {
        data += line + '\n';
    }

    source = new char[data.size() + 1];
    memcpy(source, data.c_str(), data.size() + 1);

    int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    delete[] source;

    try {
        CompileShader(shader);
    } catch (const std::exception& e) {
        glDeleteShader(shader);

        std::string msg = e.what();
        msg = "a shader failed to compile: " + msg;

        throw std::runtime_error(msg.c_str());
    }

    return shader;
}

void Shader::CompileShader(int shader) {
    int status;

    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (!status) {
        char infoLog[4096];
        int length;

        glGetShaderInfoLog(shader, 2048, &length, infoLog);

        throw std::runtime_error(std::string(infoLog, length).c_str());
    }
}

void Shader::LinkProgram(int program) {
    int status;

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (!status) {
        char infoLog[4096];
        int length;

        glGetProgramInfoLog(program, 2048, &length, infoLog);

        throw std::runtime_error(std::string(infoLog, length).c_str());
    }
}