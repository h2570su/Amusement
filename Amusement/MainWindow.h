#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

#include <glad4.6/include/glad/glad.h>

#include <iostream>
#include <string>
#include <vector>

#include <windows.h>

#include "RenderUtilities/Shader.h"
#include "LightManager.h"

#include "RenderUtilities/Texture.h"
#include "RenderUtilities/TextureCube.h"
#include "Object.H"


struct KeyMap
{
	enum
	{
		LMB = 1 << 0,
		RMB = 1 << 1,
		W = 1 << 2,
		A = 1 << 3,
		S = 1 << 4,
		D = 1 << 5,
		E = 1 << 6,
		SHIFT = 1 << 7,
		CTRL = 1 << 8,
		SPACE = 1 << 9
	};
};

class MainWindow :public Fl_Gl_Window
{
public:
	MainWindow(int x, int y, int w, int h);
	~MainWindow();

	void tick();
	void init();

	virtual void draw();
	virtual int handle(int event);

private:

	const float fov = 60.0f;
	int windowWidth;
	int windowHeight;
	bool gladInited = false;

	float charaHeight = 10.0f;
	float jump_time = -1.0f;
	const float jump_length = 0.5f;
	const float jump_height = 10.0f;

	glm::vec3 campos = glm::vec3(0, charaHeight, 250);
	glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 camup = glm::vec3(0.0f, 1.0f, 0.0f);
	float camTheta = 0.0f;
	float camPhi = 0.0f;

	float time = 0.0f;

	int pressedKeys = 0;
	bool mouseLock = false;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	UBO projViewUBO;
	UBO environmentUBO;

	Shader* shaderSimple = nullptr;
	Shader* shaderBackground = nullptr;
	Shader* shaderSurface = nullptr;
	Shader* shaderPicking = nullptr;
	Shader* shaderPostprocess = nullptr;

	mySphere sphere;

	aBox backgroundBox;
	TextureCube* backgroundTextureCube = nullptr;

	aPlane floor;
	Texture2D* floorTexture = nullptr;

private:
	void setProjection();
	void putEnvironmentOnUBO();
};

