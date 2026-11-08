#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>


GLuint LoadShaders(const std::string& vertexPath, const std::string& fragmentPath);

GLuint CompileShaderFromFile(const std::string& filePath, GLenum type);
std::string ReadFile(const std::string& filePath);
void CheckShaderErrors(GLuint shader, const std::string& name);
void CheckProgramErrors(GLuint program);