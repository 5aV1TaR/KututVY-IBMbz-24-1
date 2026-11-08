#define GLEW_DLL
#define GLFW_DLL
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"
#include <cstdio>
#include <cmath>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

// Константы приложения
const unsigned int SCR_WIDTH = 768;
const unsigned int SCR_HEIGHT = 768;
const float FOV = 45.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 100.0f;
const float CAMERA_SPEED = 0.05f;
const float SENSITIVITY = 0.05f;
const float ROTATION_SPEED = 30.0f;

// Пределы степеней свободы (в градусах)
const float DOF1_MAX = 90.0f;
const float DOF1_MIN = -90.0f;
const float DOF2_MAX = 30.0f;
const float DOF2_MIN = -30.0f;
const float DOF3_MAX = 0.0f;
const float DOF3_MIN = -20.0f;

// ТОЧНЫЕ ТОЧКИ ВРАЩЕНИЯ (Шарниры). 
const glm::vec3 PIVOT_X01 = glm::vec3(0.0025f, 0.609f, 0.029f);
const glm::vec3 PIVOT_X02 = glm::vec3(0.155f, 0.848f, 0.041f);
const glm::vec3 PIVOT_X03 = glm::vec3(0.090f, 2.442f, -0.041f);

// ОСЬ НАКЛОНА ДЛЯ РУК
const glm::vec3 ARM_ROTATION_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);

// Глобальные переменные для камеры
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool shouldClose = false;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= SENSITIVITY;
    yoffset *= SENSITIVITY;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        shouldClose = true;
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "ERROR: glewInit failed.\n");
        return 1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    printf("OpenGL версия: %s\n", glGetString(GL_VERSION));
    printf("Устройство: %s\n", glGetString(GL_RENDERER));

    Shader shader;
    if (!shader.load("model.vert", "model.frag")) {
        glfwTerminate();
        return 1;
    }

    Model model("../Graphic_LR_3/untitled.obj");
    unsigned int meshCount = model.getMeshCount();

    int idxBase = 0;
    int idxX01 = 1;
    int idxX02 = 2;
    int idxX03 = 3;

    if (meshCount < 4) {
        fprintf(stderr, "ERROR: в модели меньше 4 мешей.\n");
        glfwTerminate();
        return 1;
    }

    glm::mat4 projection = glm::perspective(glm::radians(FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);

    float dof1 = 0.0f;
    float dof2 = 0.0f;
    float dof3 = 0.0f;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window) && !shouldClose) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Камера (WASD)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += CAMERA_SPEED * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= CAMERA_SPEED * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * CAMERA_SPEED;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * CAMERA_SPEED;

        // X01: Вращение базы (Q/E)
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            float newV = dof1 + glm::radians(ROTATION_SPEED) * deltaTime;
            if (newV <= glm::radians(DOF1_MAX)) dof1 = newV;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            float newV = dof1 - glm::radians(ROTATION_SPEED) * deltaTime;
            if (newV >= glm::radians(DOF1_MIN)) dof1 = newV;
        }

        // X02: Наклон руки (R/F)
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            float newV = dof2 + glm::radians(ROTATION_SPEED) * deltaTime;
            if (newV <= glm::radians(DOF2_MAX)) dof2 = newV;
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            float newV = dof2 - glm::radians(ROTATION_SPEED) * deltaTime;
            if (newV >= glm::radians(DOF2_MIN)) dof2 = newV;
        }

        // X03: Наклон захвата (T/G)
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            float newV = dof3 + glm::radians(ROTATION_SPEED) * deltaTime;
            if (newV <= glm::radians(DOF3_MAX)) dof3 = newV;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            float newV = dof3 - glm::radians(ROTATION_SPEED) * deltaTime;
            if (newV >= glm::radians(DOF3_MIN)) dof3 = newV;
        }

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 baseMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

        // X01 — Вращение базы вокруг оси Y (поворот влево/вправо)
        glm::mat4 mX01 = baseMatrix *
            glm::translate(glm::mat4(1.0f), PIVOT_X01) *
            glm::rotate(glm::mat4(1.0f), dof1, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::translate(glm::mat4(1.0f), -PIVOT_X01);

        // X02 — НАКЛОН ВПЕРЕД/НАЗАД
        glm::mat4 mX02 = mX01 *
            glm::translate(glm::mat4(1.0f), PIVOT_X02) *
            glm::rotate(glm::mat4(1.0f), dof2, ARM_ROTATION_AXIS) *
            glm::translate(glm::mat4(1.0f), -PIVOT_X02);

        // X03 — НАКЛОН ВПЕРЕД/НАЗАД
        glm::mat4 mX03 = mX02 *
            glm::translate(glm::mat4(1.0f), PIVOT_X03) *
            glm::rotate(glm::mat4(1.0f), dof3, ARM_ROTATION_AXIS) *
            glm::translate(glm::mat4(1.0f), -PIVOT_X03);

        std::vector<glm::mat4> partMatrices(meshCount, baseMatrix);
        partMatrices[idxBase] = baseMatrix;
        partMatrices[idxX01] = mX01;
        partMatrices[idxX02] = mX02;
        partMatrices[idxX03] = mX03;

        shader.use();

        shader.setUniform("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setUniform("light.position", 1.2f, 1.0f, 2.0f);
        shader.setUniform("light.ambient", 0.1f, 0.1f, 0.1f);
        shader.setUniform("light.diffuse", 1.0f, 1.0f, 1.0f);
        shader.setUniform("light.specular", 1.0f, 1.0f, 1.0f);

        shader.setUniform("material.ambient", 1.0f, 0.5f, 0.31f);
        shader.setUniform("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.setUniform("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setUniform("material.shininess", 32.0f);

        shader.setUniform("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
        shader.setUniformMat4("projection", glm::value_ptr(projection));
        shader.setUniformMat4("view", glm::value_ptr(view));

        model.Draw(shader, partMatrices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}