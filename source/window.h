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


void clear(vec3 rgb = {0.f, 0.f, 0.f});
extern bool pressed_keys[];
void default_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void set_default_key_callback(Window* w);