#pragma once
#include "Window.h"
class Camera {
public:
	float x, y, z, pitch, yaw, moveSpeed;

	Camera(Window* window, float moveSpeed, float angleSpeed);
	void update(float dt);
	float getDirection();
private:
	const float PI;
	float angleSpeed, lastX, lastY;
	Window* window;
};