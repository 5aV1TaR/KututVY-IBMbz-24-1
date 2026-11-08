#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "Shader.h"

#include <vector>
#include <string>
#include <cstddef>

// Внешняя структура вершины: позиция + нормаль
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
        : vertices(vertices), indices(indices)
    {
        setupMesh();
    }

    void Draw(Shader& shader) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLuint)indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    GLuint VAO, VBO, EBO;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // В буфер отправляется вся структура Vertex целиком
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
            vertices.data(), GL_STATIC_DRAW);

        // В буфер индексов — только индексы
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
            indices.data(), GL_STATIC_DRAW);

        // Разбиваем буфер: координаты вершины
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(0);

        // и векторы нормалей этих же вершин
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
};
