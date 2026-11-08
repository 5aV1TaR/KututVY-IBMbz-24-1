#pragma once

#include <string>
#include "GL/glew.h"

class Shader {
public:
    Shader() = default;
    // Собирает программу из вершинного и фрагментного шейдера (файлы)
    bool load(const std::string& vertPath, const std::string& fragPath);

    void use() const;

    // Перегрузки setUniform под разные типы и количества элементов
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, int x, int y) const;
    void setUniform(const std::string& name, int x, int y, int z) const;
    void setUniform(const std::string& name, int x, int y, int z, int w) const;

    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, float x, float y) const;
    void setUniform(const std::string& name, float x, float y, float z) const;
    void setUniform(const std::string& name, float x, float y, float z, float w) const;

    void setUniformMat3(const std::string& name, const float* m) const;
    void setUniformMat4(const std::string& name, const float* m) const;

    GLuint id() const { return program; }

private:
    bool compile(GLuint shader, const std::string& type) const;
    std::string readFile(const std::string& path) const;

    GLuint program = 0;
};
