#pragma once
#include <GLFW\glfw3.h>
class Window {
public:
	Window(const int width, const int height, const bool fullscreen, const int maxFps, const char* title);
	~Window();
	float getMousePositionX();
	float getMousePositionY();
	bool checkKey(const unsigned int key);
	float getWindowRatio();
	bool isClosed();
	void update();
	int getWidth();
	int getHeight();
private:
	GLFWwindow* window;
	int width, height;
};