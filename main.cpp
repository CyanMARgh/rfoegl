#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <fstream>
#include <cstdint>
#include "utils.h"
#include <stddef.h>
#include <SOIL/SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef uint64_t u64;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint8_t u8;
typedef int8_t s8;

std::string read_entire_file(const std::string& path) {
	std::ifstream file(path);
	if(!file) {
		printf("failed to open file: %s\n", path.c_str());
		exit(-1);
	}
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
		exit(-1);
	}
	printf("shader compiled succesfully (path: %s)\n", path.c_str());
	return shader;
}
u32 get_shader_program(const std::string& path_vert, const std::string& path_frag) {
	u32 vertex_shader = get_shader(path_vert, GL_VERTEX_SHADER);
	u32 fragment_shader = get_shader(path_frag, GL_FRAGMENT_SHADER);
	u32 shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	puts("shader linked succesfully");
	return shader_program;
}
GLFWwindow* init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "rfoegl", nullptr, nullptr);
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
	return window;
}
u32 load_texture(const std::string& path) {
	u32 texture;
	glGenTextures(1, &texture);
	
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int tex_width, tex_height;
	u8* image_data = SOIL_load_image(path.c_str(), &tex_width, &tex_height, 0, SOIL_LOAD_RGB);
	printf("texture (%s): %lx, %d, %d, %s\n", path.c_str(), (u64)image_data, tex_width, tex_height, SOIL_last_result());
	if(!image_data) exit(-1);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image_data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

int main() {
	auto window = init();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);  
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);

	u32 shader_program = get_shader_program("vertex.vert", "fragment.frag");

	struct Point {
		vec3 pos; 
		vec2 uv = {0.f, 0.f};
	};
	Point vertices[] = {
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},

		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},

		{{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},

		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},

		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},

		{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}}
    };
    glm::vec3 cube_positions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f), 
		glm::vec3( 2.0f,  5.0f, -15.0f), 
		glm::vec3(-1.5f, -2.2f, -2.5f),  
		glm::vec3(-3.8f, -2.0f, -12.3f),  
		glm::vec3( 2.4f, -0.4f, -3.5f),  
		glm::vec3(-1.7f,  3.0f, -7.5f),  
		glm::vec3( 1.3f, -2.0f, -2.5f),  
		glm::vec3( 1.5f,  2.0f, -2.5f), 
		glm::vec3( 1.5f,  0.2f, -1.5f), 
		glm::vec3(-1.3f,  1.0f, -1.5f) 
    };
	// u32 indices[] = {
	// 	0, 1, 2,
	// 	3, 4, 5,
	// };

	u32 texture = load_texture("wallpaper.png");

	u32 VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, pos));
	glEnableVertexAttribArray(0);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));
	// glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, uv));
	glEnableVertexAttribArray(1);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	// u32 uloc_time = glGetUniformLocation(shader_program, "u_time");
	u32 uloc_texture = glGetUniformLocation(shader_program, "u_texture");
	u32 uloc_transform = glGetUniformLocation(shader_program, "u_transform");

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		float time = glfwGetTime();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(uloc_texture, 0);
		glUseProgram(shader_program);
		// glUniform1f(uloc_time, time);

		glm::mat4 view(1.f), projection(1.f);
		view = glm::translate(view, glm::vec3(0.f, 0.f, -4.f));
		projection = glm::perspective(45.f, (float)width/(float)height, .1f, 100.f);

		glBindVertexArray(VAO);
		for(u32 i = 0; i < 10; i++) {			
			glm::mat4 model(1.f);
			model = glm::translate(model, cube_positions[i]);
			model = glm::rotate(model, 2.f * sqrtf(i + 1.f) * time, glm::vec3(1.f, .3f, .5f));
			glm::mat4 transform = projection * view * model;

			glUniformMatrix4fv(uloc_transform, 1, GL_FALSE, glm::value_ptr(transform));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);		
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}