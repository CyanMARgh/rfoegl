#include "window.h"

#include <cstdio>

Window::Window(u32 _width, u32 _height) {
	width = _width, height = _height;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, "rfoegl", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	if(!window) {
		puts("Failed to create GLFW window\n");
		glfwTerminate();
		exit(-1);
	}
	if(glewInit() != GLEW_OK) {
		puts("Failed to initialize GLEW\n");
		exit(-1);
	}
	glfwGetFramebufferSize(window, &width, &height);  
	glViewport(0, 0, width, height);
}
Window::~Window() {
	glfwTerminate();
}
