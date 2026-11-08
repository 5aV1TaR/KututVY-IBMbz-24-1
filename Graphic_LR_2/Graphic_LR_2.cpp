#define GLEW_DLL
#define GLFW_DLL
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <iostream>
#include "ShaderLoader.h"

#include <Windows.h>

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
    GLFWwindow* window = glfwCreateWindow(512, 512, "MainWindow", NULL, NULL);
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

    // вершины квадрата (4 точки, против часовой стрелки)
    float points[] =
    {
        -0.5f, -0.5f,   // 0: левый нижний
         0.5f, -0.5f,   // 1: правый нижний
         0.5f,  0.5f,   // 2: правый верхний
        -0.5f,  0.5f    // 3: левый верхний
    };

    // индексы двух треугольников, образующих квадрат
    GLuint indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    // генерация буферов
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // привязка буферов
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // отвязка буферов
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    /*
            Подключение шейдеров
            (версия с подключением из файлов)
                                                    */

    GLuint shader_program = LoadShaders("Shaders/vertex.txt", "Shaders/fragment.txt");
    if (shader_program == 0)
    {
        std::cerr << "Failed to load shaders!" << std::endl;
        glfwTerminate();
        return -1;
    }

    /*
            Конец кода для подключения шейдеров
                                                    */

    // получение адреса для изменения цвета
    GLint colourLocation = glGetUniformLocation(shader_program, "ourColour");

    // тело цикла отрисовки
    while (!glfwWindowShouldClose(window))
    {
        float timeValue = glfwGetTime();

        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);

        glUniform4f(
            colourLocation,
            (float)0.3f + 0.5f * sin(1.5f * timeValue),
            (float)0.6f + 0.3f * sin(3.0f * timeValue),
            (float)1.0f - 0.8f * sin(timeValue + 0.5f * 3.1415926535f),
            1.0f);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}