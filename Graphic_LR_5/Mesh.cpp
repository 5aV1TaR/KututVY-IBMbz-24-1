#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
{
	this->vertices = vertices;
	this->indices = indices;

	setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(
		GL_ARRAY_BUFFER, 
		vertices.size() * sizeof(Vertex), 
		&vertices[0], 
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		indices.size() * sizeof(unsigned int), 
		&indices[0], 
		GL_STATIC_DRAW);

	glVertexAttribPointer(
		0, 
		3, 
		GL_FLOAT, 
		GL_FALSE, 
		sizeof(Vertex), 
		(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1, 
		3, 
		GL_FLOAT, 
		GL_FALSE, 
		sizeof(Vertex), 
		(void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
}

void Mesh::Draw()
{
	glBindVertexArray(VAO);

	glDrawElements(
		GL_TRIANGLES, 
		static_cast<unsigned int>(indices.size()), 
		GL_UNSIGNED_INT, 
		0);

	glBindVertexArray(0);
}