#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"

struct Window {
	GLFWwindow* window;
	int width, height;
	
	Window(u32 _width, u32 _height);
	~Window();
};