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

// Константы приложения
const float SCR_WIDTH = 512.0f;
const float SCR_HEIGHT = 512.0f;
const float FOV = 45.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 100.0f;
const float CAMERA_SPEED = 0.05f;
const float SENSITIVITY = 0.05f;
const float ROTATION_SPEED = 10.0f;

// Глобальные переменные для камеры
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 512.0f / 2.0f;
float lastY = 512.0f / 2.0f;
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
    // Инициализация GLFW
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    // Версия OpenGL 4.6 + core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(768, 768, "OpenGL Window", NULL, NULL);

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

    // Настройка обработки событий
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    const GLubyte* version_str = glGetString(GL_VERSION);
    const GLubyte* device_str = glGetString(GL_RENDERER);
    printf("OpenGL версия: %s\n", version_str);
    printf("Устройство: %s\n", device_str);

    // Загрузка шейдеров из файлов
    Shader shader;
    if (!shader.load("model.vert", "model.frag")) {
        glfwTerminate();
        return 1;
    }

    // Загрузка модели через ASSIMP
    Model model("../Graphic_LR_3/untitled.obj");

    glm::mat4 projection = glm::perspective(
        glm::radians(FOV),
        SCR_WIDTH / SCR_HEIGHT,
        NEAR_PLANE,
        FAR_PLANE
    );

    // Настройки OpenGL
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
 
    // Основной цикл
    while (!glfwWindowShouldClose(window) && !shouldClose) {
        // Обработка ввода (WASD)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += CAMERA_SPEED * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= CAMERA_SPEED * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * CAMERA_SPEED;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * CAMERA_SPEED;

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float timeValue = glfwGetTime();

        float r = sin(timeValue * 2.0f) * 0.5f + 0.5f;
        float g = 0.7f;
        float b = cos(timeValue * 3.0f) * 0.5f + 0.5f;

        // Матрица модели: вращение, чтобы было видно объём
        // UPD: Угловую скорость вращения замедлил до 10 гр/сек для удобства просмотра и вынес в константу ROTATION_SPEED
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::rotate(modelMatrix, timeValue * glm::radians(ROTATION_SPEED),
            glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

        // Нормальная матрица: компенсирует искажение нормалей при трансформациях
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

        shader.use();

        // Цвет света
        shader.setUniform("lightColor", r, g, b);

        // Параметры источника света
        shader.setUniform("light.position", 1.2f, 1.0f, 2.0f);
        shader.setUniform("light.ambient", 0.1f, 0.1f, 0.1f);
        shader.setUniform("light.diffuse", 1.0f, 1.0f, 1.0f);
        shader.setUniform("light.specular", 1.0f, 1.0f, 1.0f);

        // Свойства материала
        shader.setUniform("material.ambient", 1.0f, 0.5f, 0.31f);
        shader.setUniform("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.setUniform("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setUniform("material.shininess", 32.0f);

        // Позиция камеры
        shader.setUniform("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

        // Матрицы
        shader.setUniformMat3("normalMatrix", glm::value_ptr(normalMatrix));
        shader.setUniformMat4("projection", glm::value_ptr(projection));
        shader.setUniformMat4("view", glm::value_ptr(view));
        shader.setUniformMat4("model", glm::value_ptr(modelMatrix));

        model.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
