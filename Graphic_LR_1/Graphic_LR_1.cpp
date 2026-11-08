#define GLEW_DLL
#define GLFW_DLL
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <iostream>

int main()
{
    // Проверка инициализации библиотеки GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: could not start GLFW3. \n");
        return 1;
    }

    // задание версии библиотеки GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    // тело цикла отрисовки
    while (!glfwWindowShouldClose(window))
    {
        // Цвет фона: (0.5, 0.5, 1.0)
        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Цвет квадрата: (1.0, 0.5, 0.0)
        glColor3f(1.0f, 0.5f, 0.0f);

        glBegin(GL_TRIANGLES);

        // Первый треугольник квадрата
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);

        // Второй треугольник квадрата
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);

        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}