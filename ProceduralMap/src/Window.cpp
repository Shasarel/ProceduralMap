#include "Window.h"
#include <GLFW\glfw3.h>
#include "Logger.h"

Window::Window(const int width, const int height, const bool fullscreen, const int maxFps, const char* title)
{
	Logger::printInfo("Creating Window");
	if (!glfwInit())
	{
		Logger::printError("Failed to initialize GLFW");
	}
	else 
	{

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SRGB_CAPABLE, true);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		if (fullscreen) {
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			window = glfwCreateWindow(mode->width, mode->height, title, NULL, NULL);
			this->width = mode->width;
			this->height = mode->height;

		}
		else {
			glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
			window = glfwCreateWindow(width, height, title, NULL, NULL);
			this->width = width;
			this->height = height;
		}

		if (window == NULL) {
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);
		glfwSwapInterval((int)(100.0 / maxFps));

		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

Window::~Window()
{
	glfwTerminate();
}

float Window::getMousePositionX()
{
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return (float)xPos;
}

float Window::getMousePositionY()
{
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	return (float)yPos;
}

bool Window::checkKey(const unsigned int key)
{
	return (glfwGetKey(window, key) == GLFW_PRESS);
}

float Window::getWindowRatio()
{
	return width / (float) height;
}

bool Window::isClosed()
{
	return glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window);
}

void Window::update()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

int Window::getWidth()
{
	return width;
}

int Window::getHeight()
{
	return height;
}
