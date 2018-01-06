#include <GL/glew.h>
#include <chrono>
#include <string>
#include <Windows.h>
#include <Psapi.h>
#include "TextRenderer.h"
#include "Camera.h"
#include "TextureManager.h"
#include "Scene.h"
#include "Logger.h"
#include <ctime>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>



/*ToDo

Ulepszone oœwietlenie
algorytmy korozji
cienie

config z pliku

Mg³a przestrzenna, chmury
skybox + przesuwaj¹ce siê Ÿród³o œwiat³a
dzieñ/noc
fizyka
woda
obiekty 3d(np drzewa, kamienie)
*/

const float MAP_SIZE = 3;
const float MAP_HEIGHT = 450.0f;
const int CHUNK_RESOLUTION = 219;//max 255
const int CHUNK_COUNT = 10;
const float ANGLE_SPEED = 0.1f;
const int MAX_FPS = 6000;

Window* window;
TextRenderer* textRenderer;
Camera* camera;
Scene* scene;

bool showInfo = true;
bool printScreenPressed = false;

bool init() {
	//	FreeConsole();
	Logger::openFile();
	Logger::printInfo("Starting");
	window = new Window(800, 600, true, MAX_FPS, "Mapa Proceduralna");
	if (glewInit() != GLEW_OK) {
		Logger::printError("Failed to initialize GLEW");
		return false;
	}
	textRenderer = new TextRenderer();

	scene = new Scene(window->getWindowRatio());
	scene->loadMap(CHUNK_RESOLUTION, CHUNK_COUNT, MAP_SIZE, MAP_HEIGHT);

	camera = new Camera(window, 20.0f, ANGLE_SPEED);
	camera->z = 400;
	camera->pitch = -90;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CW);

	return true;
}

void checkKeys()
{
	if(window->checkKey(GLFW_KEY_F3))
		showInfo = true;

	if (window->checkKey(GLFW_KEY_F4))
		showInfo = false;

	if (window->checkKey(GLFW_KEY_LEFT_SHIFT))
		camera->moveSpeed = 400;
	else
		camera->moveSpeed = 20;

	if (window->checkKey(GLFW_KEY_PRINT_SCREEN) && !printScreenPressed) {
		int width = window->getWidth(), height = window->getHeight();

		std::chrono::system_clock::time_point p = std::chrono::system_clock::now();

		std::time_t t = std::chrono::system_clock::to_time_t(p);

		std::tm now_tm;

		localtime_s(&now_tm, &t);
		std::string datetime = "screenshots/" + std::to_string(now_tm.tm_year + 1900) + "-" + std::to_string(now_tm.tm_mon) + "-" + std::to_string(now_tm.tm_mday) + " " + std::to_string(now_tm.tm_hour) + "-" + std::to_string(now_tm.tm_min) + "-" + std::to_string(now_tm.tm_sec) + ".jpg";
		printf(datetime.c_str());
		unsigned char* pixels = new unsigned char[width * height * 3];
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		TextureManager::flipImage(pixels, width, height, 3);
		stbi_write_jpg(datetime.c_str(), width, height, 3, pixels, 100);
		printScreenPressed = true;
		delete[] pixels;
	}
	if (!window->checkKey(GLFW_KEY_PRINT_SCREEN)) {
		printScreenPressed = false;
	}
}

int main()
{
	if (!init())
		return -1;

	auto lastUpdate = std::chrono::system_clock::now();
	auto now = std::chrono::system_clock::now();

	float speed = 0;
	int updates = 0;
	float fpsTimer = 0;
	int fps = 0;
	PROCESS_MEMORY_COUNTERS memCounter;
	do {
		BOOL result = GetProcessMemoryInfo(GetCurrentProcess(),
			&memCounter,
			sizeof(memCounter));
		now = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = now - lastUpdate;
		float dt = diff.count();
		fpsTimer += dt;
		if (fpsTimer > 1) {
			fps = updates;
			updates = 0;
			fpsTimer = 0;
		}
		lastUpdate = now;
		checkKeys();
		scene->render(camera);
		if (showInfo) {
			textRenderer->renderText(("Mem: " + std::to_string(memCounter.WorkingSetSize/(1024*1024)) + "MB").c_str(), -0.99f, 0.95f, 0.0005f, 0.001f);
			textRenderer->renderText(("FPS: " + std::to_string(fps)).c_str(), -0.99f, 0.90f, 0.0005f, 0.001f);
			textRenderer->renderText(("X: " + std::to_string(camera->x)).c_str(), -0.99f, 0.85f, 0.0005f, 0.001f);
			textRenderer->renderText(("Y: " + std::to_string(camera->y)).c_str(), -0.99f, 0.80f, 0.0005f, 0.001f);
			textRenderer->renderText(("Z: " + std::to_string(camera->z)).c_str(), -0.99f, 0.75f, 0.0005f, 0.001f);
			textRenderer->renderText(("Dir: " + std::to_string(camera->getDirection())).c_str(), -0.99f, 0.70f, 0.0005f, 0.001f);
		}
		updates++;
		window->update();
		camera->update(dt);
	}

	while (!window->isClosed());
	Logger::printInfo("Closing program");
	delete window;
	delete scene;
	delete textRenderer;
	delete camera;
	TextureManager::deleteTextureManager();
	Logger::printInfo("Bye");
	Logger::closeFile();

	return 0;
}