#include "window.h"

#include <cstdio>

#include "primitives.h"
#include "texture.h"

struct Glfw_Finalizer {
	bool glfw_on = false;
	~Glfw_Finalizer() {
		if(glfw_on) {
			glfwTerminate();
		}
	}
} glfw_finalizer;

void glfw_init() {
	if(!glfw_finalizer.glfw_on) {
		glfw_finalizer.glfw_on = true;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_SAMPLES, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	}
}

Window::Window(u32 _width, u32 _height) {
	width = _width, height = _height;

	glfw_init();

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
	glfwDestroyWindow(window);
	clear_texture_manager();
	clear_primitives();
}


void clear(vec3 rgb) {
	glClearColor(rgb.r, rgb.g, rgb.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool pressed_keys[1024] = {false};
std::function<void(int)> click_request;

void default_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if(action == GLFW_PRESS) {
		pressed_keys[key] = true;
	} else if(action == GLFW_RELEASE) {
		pressed_keys[key] = false;
		if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
		if(click_request != nullptr) click_request(key);
	}
}
void set_default_key_callback(Window* w, std::function<void(int)> _click_request) {
	glfwSetKeyCallback(w->window, default_key_callback);
	click_request = _click_request;
}