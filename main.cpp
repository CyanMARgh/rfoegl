#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <fstream>
#include <cstdint>
#include "utils.h"
#include <stddef.h>
#include <SOIL/SOIL.h>
#include <tuple>
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
struct Window {
	GLFWwindow* window;
	int width, height;
	
	Window() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		window = glfwCreateWindow(800, 600, "rfoegl", nullptr, nullptr);
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
		glEnable(GL_DEPTH_TEST);
	}
	~Window() {
		glfwTerminate();
	}
};
struct Texture {
	u32 id;
	int width, height;
};
Texture load_texture(const std::string& path) {
	Texture texture;
	glGenTextures(1, &texture.id);
	
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	u8* image_data = SOIL_load_image(path.c_str(), &texture.width, &texture.height, 0, SOIL_LOAD_RGB);

	printf("texture (%s): %lx, %d, %d, %s\n", path.c_str(), (u64)image_data, texture.width, texture.height, SOIL_last_result());
	if(!image_data) exit(-1);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image_data);
	glBindTexture(GL_TEXTURE_2D, 0);


	u8* image_data_new = new u8[texture.width * texture.height * 3];	
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glGetTexImage(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		image_data);
	SOIL_save_image("output_image.png", SOIL_SAVE_TYPE_BMP, texture.width, texture.height, 3, image_data_new);

	SOIL_free_image_data(image_data_new);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

struct Point {
	vec3 pos; 
	vec2 uv = {0.f, 0.f};
};
struct Mesh {
	u32 VBO, VAO, EBO;
	Mesh(Point* points, u32 points_count, u32* indices, u32 indices_count) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * points_count, points, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, pos));
		glEnableVertexAttribArray(0);
		// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, color));
		// glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, uv));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(u32), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
	~Mesh() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
};

struct Camera {
	glm::mat4 translation{1.f};
	glm::mat4 rotation{1.f};
};

Camera main_camera;
bool pressed_keys[1024] = {false};

void move_camera(float delta_time) {
	GLfloat cam_speed = 5.f, rot_speed = 2.0f;
	glm::vec4 direction = {0.f, 0.f, 0.f, 0.f};
	float angle = 0.f;
	if(pressed_keys[GLFW_KEY_W]) direction.z -= 1.f;
	if(pressed_keys[GLFW_KEY_S]) direction.z += 1.f;
	if(pressed_keys[GLFW_KEY_A]) direction.x -= 1.f;
	if(pressed_keys[GLFW_KEY_D]) direction.x += 1.f;	
	if(pressed_keys[GLFW_KEY_Q]) angle += 1.f;
	if(pressed_keys[GLFW_KEY_E]) angle -= 1.f;
	direction = main_camera.rotation * direction * cam_speed * delta_time;
	glm::vec3 direction3 = {direction.x, direction.y, direction.z};

	main_camera.translation = glm::translate(main_camera.translation, direction3);
	main_camera.rotation = glm::rotate(main_camera.rotation, rot_speed * angle * delta_time, glm::vec3(0.f, 1.f, 0.f));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if(action == GLFW_PRESS) {
		pressed_keys[key] = true;
	} else if(action == GLFW_RELEASE) {
		pressed_keys[key] = false;
	}
}

int main() {
	Window main_window;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	glfwSetKeyCallback(main_window.window, key_callback);

	const u32 VERTICES_COUNT = 24, INDICES_COUNT = 36;
	Point vertices[VERTICES_COUNT] = {
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},

		{{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},

		{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}},
    };
	u32 indices[] = {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};
    const u32 CUBE_NUM = 10;
    glm::vec3 cube_positions[CUBE_NUM];
    for(u32 i = 0; i < CUBE_NUM; i++) {
		cube_positions[i] = glm::vec3(randf() * 4.f - 2.f, randf() * 4.f - 2.f, randf() * -5.f);
    }

    Mesh mesh(vertices, VERTICES_COUNT, indices, INDICES_COUNT);

	u32 shader_program = get_shader_program("vertex.vert", "fragment.frag");
	auto texture = load_texture("chio_rio.png");

	// u32 uloc_time = glGetUniformLocation(shader_program, "u_time");
	u32 uloc_texture = glGetUniformLocation(shader_program, "u_texture");
	u32 uloc_transform = glGetUniformLocation(shader_program, "u_transform");

	float prev_time = glfwGetTime();
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(delta_time); prev_time = new_time;
		glfwPollEvents();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glUniform1i(uloc_texture, 0);
		glUseProgram(shader_program);
		// glUniform1f(uloc_time, time);

		glm::mat4 view(1.f), projection(1.f), scale(1.f);
		scale = glm::scale(scale, glm::vec3(1.f));
		projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);

		glBindVertexArray(mesh.VAO);
		for(u32 i = 0; i < CUBE_NUM; i++) {			
			glm::mat4 model(1.f);
			model = glm::translate(model, cube_positions[i]);
			float angle = (float)(2.f * pow(i + 2.f, .2f) * new_time);
			model = glm::rotate(model, angle, glm::vec3(1.f, .3f, .5f));
			glm::mat4 transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation) * model * scale;

			glUniformMatrix4fv(uloc_transform, 1, GL_FALSE, glm::value_ptr(transform));
			// glDrawArrays(GL_TRIANGLES, 0, 36);
			glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);

		glfwSwapBuffers(main_window.window);		
	}
	return 0;
}