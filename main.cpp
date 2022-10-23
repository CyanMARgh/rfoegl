#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include "utils.h"
#include <stddef.h>
#include <SOIL/SOIL.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint8_t u8;
typedef int8_t s8;

std::string read_entire_file(const std::string& path) {
	std::ifstream file(path);
	if(!file) printf("failed to open file: %s\n", path.c_str());
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}
u32 get_shader(const std::string& path, decltype(GL_VERTEX_SHADER) type) {
	u32 shader = glCreateShader(type);
	auto _shader_source = read_entire_file(path);
	auto shader_source = _shader_source.c_str();
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);
	s32 succes;
	s8 info_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &succes);
	if(!succes) {
		glGetShaderInfoLog(shader, 512, NULL, (GLchar*)info_log);
		printf("failed to compile shader (path: %s): %s\n", path.c_str(), info_log);
	}
	return shader;
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "rfoegl", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	if(!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if(glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);  
	glViewport(0, 0, width, height);

	u32 vertex_shader = get_shader("vertex.vert", GL_VERTEX_SHADER);
	u32 fragment_shader = get_shader("fragment.frag", GL_FRAGMENT_SHADER);

	u32 shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	struct Point {
		vec3 pos, color = {0.f, 0.f, 0.f}; 
		vec2 uv = {0.f, 0.f};
	};
	Point vertices[] = {
		{{0.5f,  0.5f, 0.0f}, {1.f,0.f,0.f}, {1.f, 1.f}},
		{{0.5f, -0.5f, 0.0f}, {0.f,1.f,0.f}, {1.f, 0.f}},
		{{-0.5f, -0.5f, 0.0f}, {0.f,0.f,1.f}, {0.f, 0.f}},
		{{-0.5f,  0.5f, 0.0f}, {1.f,1.f,1.f}, {0.f, 1.f}}
	};
	u32 indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glEnable(GL_TEXTURE_2D);

	u32 texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	int tex_width, tex_height;
	u8* image_data = SOIL_load_image("wallpaper.png", &tex_width, &tex_height, 0, SOIL_LOAD_RGB);
	printf("%lx, %d, %d, %s\n", (u64)image_data, tex_width, tex_height, SOIL_last_result());

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image_data); // why not just delete[] or free?
	glBindTexture(GL_TEXTURE_2D, 0);

	u32 VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, pos));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, uv));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		glUniform1f(glGetUniformLocation(shader_program, "u_time"), glfwGetTime());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shader_program, "u_texture"), 0);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);		
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}