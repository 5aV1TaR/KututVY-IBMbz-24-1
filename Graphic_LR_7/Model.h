#pragma once

#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>
#include <string>
#include <cstdio>

class Model {
public:
    Model(const std::string& path) {
        loadModel(path);
    }

    unsigned int getMeshCount() const {
        return (unsigned int)meshes.size();
    }

    // Индекс меша по имени — чтобы в main() не зависеть от порядка в OBJ
    int getMeshIndex(const std::string& name) const {
        for (unsigned int i = 0; i < meshNames.size(); i++)
            if (meshNames[i] == name)
                return (int)i;
        return -1;
    }

    // Отрисовка с индивидуальной матрицей для каждого звена
    void Draw(Shader& shader, const std::vector<glm::mat4>& partMatrices) {
        shader.use();
        for (unsigned int i = 0; i < meshes.size(); i++) {
            glm::mat4 modelMatrix = partMatrices[i];
            // Нормальная матрица: компенсирует искажение нормалей при трансформациях
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

            shader.setUniformMat3("normalMatrix", glm::value_ptr(normalMatrix));
            shader.setUniformMat4("model", glm::value_ptr(modelMatrix));

            meshes[i].Draw(shader);
        }
    }

private:
    std::vector<Mesh> meshes;
    std::vector<std::string> meshNames;
    std::string directory;

    void loadModel(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            printf("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            printf("Mesh %u: \"%s\" (%u vertices)\n",
                (unsigned int)meshes.size(),
                mesh->mName.C_Str(),
                (unsigned int)mesh->mNumVertices);
            meshNames.push_back(mesh->mName.C_Str());
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene);
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z);
            if (mesh->HasNormals())
                vertex.Normal = glm::vec3(mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z);
            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        return Mesh(vertices, indices);
    }
};
