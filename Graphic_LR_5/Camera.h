#pragma once

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"



struct GLFWwindow;

class Camera
{
	public:
		Camera();

		void MoveCamera(GLFWwindow* window);
		void RotateCamera(double xposIn, double yposIn);
		void UpdateCameraDirection();
		glm::mat4 GetViewMatrix() const;

		glm::vec3 cameraPos;
		glm::vec3 cameraFront;
		glm::vec3 cameraUp;

		float yaw;
		float pitch;


	private:
		float lastX;
		float lastY;

		float cameraSpeed;
		float cameraSensitivity;

		bool firstMouse;
};

