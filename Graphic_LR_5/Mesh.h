#pragma once

#include <glm.hpp>
#include <vector>

#include "GL/glew.h"

using std::vector;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
};


class Mesh
{
	public:
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		GLuint VAO;

		Mesh(vector<Vertex> vertices, vector<unsigned int> indices);

		void Draw();


	private:
		GLuint VBO, EBO;

		void setupMesh();
};

