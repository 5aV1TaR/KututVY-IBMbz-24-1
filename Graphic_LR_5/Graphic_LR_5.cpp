#define GLEW_DLL
#define GLFW_DLL
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <Windows.h>
#include <iostream>

#include "Camera.h"
#include "ShaderLoader.h"
#include "Model.h"




// переменные камеры
Camera* camera = nullptr;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    camera->RotateCamera(xposIn, yposIn);
}



int main()
{
    // Проверка инициализации библиотеки GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: could not start GLFW3. \n");
        return 1;
    }

    // задание версии библиотеки GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // создание контекста окна
    GLFWwindow* window = glfwCreateWindow(1024, 768, "MainWindow", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    // инициализация GLEW
    GLenum ret = glewInit();
    if (GLEW_OK != ret)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
        return 1;
    }

    const GLubyte* version_str = glGetString(GL_VERSION);
    const GLubyte* device_str = glGetString(GL_RENDERER);

    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("This version OpenGL running is %s\n", version_str);
    printf("This device OpenGL running is %s\n", device_str);

    // Подключение шейдеров                                      

    GLuint shader_program = LoadShaders("Shaders/vertex.txt", "Shaders/fragment.txt");
    if (shader_program == 0)
    {
        std::cerr << "Failed to load shaders!" << std::endl;
        glfwTerminate();
        return -1;
    }

    Model model("../Graphic_LR_3/untitled.obj");


    // получение адреса для изменения цвета
    GLint colourLocation = glGetUniformLocation(shader_program, "lightColour");

    // получение адресов для вершинного шейдера
    GLint projectionLocation = glGetUniformLocation(shader_program, "projection");
    GLint viewLocation = glGetUniformLocation(shader_program, "view");
    GLint modelLocation = glGetUniformLocation(shader_program, "model");
    GLint transformLocation = glGetUniformLocation(shader_program, "transform");

    // настройка камеры

    glfwSetCursorPosCallback(window, mouse_callback);

    Camera cameraObject;
    camera = &cameraObject;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // матрицы проекции и вида
    glm::mat4 projection = glm::perspective(
        glm::radians(90.0f),
        1.333f,
        0.1f,
        100.0f);


    // тело цикла отрисовки
    while (!glfwWindowShouldClose(window))
    {
        cameraObject.MoveCamera(window);

        // отрисовка

        glUniform3f(
            colourLocation,
            0.8f,
            0.5f,
            0.3f
        );

        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);


        glm::mat4 view = cameraObject.GetViewMatrix();

        glm::mat4 modelMatrix = glm::mat4(1.0f);

        glUniformMatrix4fv(
            projectionLocation,
            1,
            GL_FALSE,
            glm::value_ptr(projection));

        glUniformMatrix4fv(
            viewLocation,
            1,
            GL_FALSE,
            glm::value_ptr(view));

        glUniformMatrix4fv(
            modelLocation,
            1,
            GL_FALSE,
            glm::value_ptr(modelMatrix));


        model.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

