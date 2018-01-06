#include "Camera.h"
#include <math.h>
#include "Logger.h"

Camera::Camera(Window* window, float moveSpeed, float angleSpeed) :
	moveSpeed(moveSpeed),
	angleSpeed(angleSpeed),
	window(window),
	PI(3.14159265f)
{
	Logger::printInfo("Creating camera");
}

void Camera::update(float dt)
{
	if (window->checkKey(GLFW_KEY_W)) {
		y += moveSpeed * cosf(yaw / 180.0f * PI) * dt;
		x += moveSpeed * sinf(yaw / 180.0f * PI) * dt;
		z -= moveSpeed * cosf(pitch / 180.0f * PI) * dt;
	} else 
	if (window->checkKey(GLFW_KEY_S)) {
		y -= moveSpeed * cosf(yaw / 180.0f * PI) * dt;
		x -= moveSpeed * sinf(yaw / 180.0f * PI) * dt;
		z += moveSpeed * cosf(pitch / 180.0f * PI) * dt;
	}else 
	if (window->checkKey(GLFW_KEY_A)) {
		y += moveSpeed * cosf((yaw - 90) / 180.0f * PI) * dt;
		x += moveSpeed * sinf((yaw - 90)/ 180.0f * PI) * dt;
	}else 
	if (window->checkKey(GLFW_KEY_D)) {
		y -= moveSpeed * sinf(yaw / 180.0f * PI) * dt;
		x += moveSpeed * cosf(yaw / 180.0f * PI) * dt;
	}
	if (lastX != -1) {
		pitch += angleSpeed * (window->getMousePositionY() - lastY);
		if (pitch > 0)
			pitch = 0;
		if (pitch <-180)
			pitch = -180;

		yaw += angleSpeed * (window->getMousePositionX() - lastX);
		if (yaw < -180)
			yaw = yaw + 360;
		if (yaw > 180)
			yaw = yaw - 360;
	}
	lastX = window->getMousePositionX();
	lastY = window->getMousePositionY();
}

float Camera::getDirection()
{
	return (yaw / 90.0f) + 3.0f;
}
