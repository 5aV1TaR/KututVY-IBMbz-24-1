#include "ShaderLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>


GLuint LoadShaders(const std::string& vertexPath, const std::string& fragmentPath)
{
	GLuint vertexShader = CompileShaderFromFile(vertexPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = CompileShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);

	GLint vertexStatus, fragmentStatus;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);

	if (!vertexStatus || !fragmentStatus)
	{
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return 0;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	CheckProgramErrors(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus)
	{
		glDeleteProgram(program);
		return 0;
	}

	std::cout << "Shader program loaded successfully!" << std::endl;
	return program;
}



GLuint CompileShaderFromFile(const std::string& filePath, GLenum type)
{
	std::string source = ReadFile(filePath);
	if (source.empty())
		return 0;

	GLuint shader = glCreateShader(type);
	const char* sourcePtr = source.c_str();
	glShaderSource(shader, 1, &sourcePtr, NULL);
	glCompileShader(shader);

	std::string shaderName = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
	CheckShaderErrors(shader, shaderName);

	return shader;
}

std::string ReadFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "Error: Cannot open file: " << filePath << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void CheckShaderErrors(GLuint shader, const std::string& name)
{
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infolog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infolog);
		std::cerr
			<< "Error: Shader compilation failed [ " << name << " ]:\n"
			<< infolog << std::endl;
	}
}

void CheckProgramErrors(GLuint program) 
{
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[1024];
		glGetProgramInfoLog(program, 1024, NULL, infoLog);
		std::cerr << "Error: Program linking failed:\n" << infoLog << std::endl;
	}
}
