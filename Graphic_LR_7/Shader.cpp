#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

std::string Shader::readFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("ERROR: cannot open shader file: " + path);
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool Shader::compile(GLuint shader, const std::string& type) const {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: "
            << type << "\n" << infoLog << "\n";
        return false;
    }
    return true;
}

bool Shader::load(const std::string& vertPath, const std::string& fragPath) {
    try {
        std::string vertCode = readFile(vertPath);
        std::string fragCode = readFile(fragPath);
        const char* vSrc = vertCode.c_str();
        const char* fSrc = fragCode.c_str();

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vSrc, NULL);
        glCompileShader(vs);
        if (!compile(vs, "VERTEX")) return false;

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fSrc, NULL);
        glCompileShader(fs);
        if (!compile(fs, "FRAGMENT")) return false;

        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
            return false;
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return false;
    }
}

void Shader::use() const {
    glUseProgram(program);
}

void Shader::setUniform(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, int x, int y) const {
    glUniform2i(glGetUniformLocation(program, name.c_str()), x, y);
}

void Shader::setUniform(const std::string& name, int x, int y, int z) const {
    glUniform3i(glGetUniformLocation(program, name.c_str()), x, y, z);
}

void Shader::setUniform(const std::string& name, int x, int y, int z, int w) const {
    glUniform4i(glGetUniformLocation(program, name.c_str()), x, y, z, w);
}

void Shader::setUniform(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(program, name.c_str()), x, y);
}

void Shader::setUniform(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
}

void Shader::setUniform(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
}


void Shader::setUniformMat3(const std::string& name, const float* m) const {
    GLint loc = glGetUniformLocation(program, name.c_str());
    if (loc == -1) {
        return;
    }
    glUniformMatrix3fv(loc, 1, GL_FALSE, m);
}

void Shader::setUniformMat4(const std::string& name, const float* m) const {
    GLint loc = glGetUniformLocation(program, name.c_str());
    if (loc == -1) {
        return;
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, m);
}
