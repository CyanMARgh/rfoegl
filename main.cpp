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
#include <vector>
#include <functional>

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
u32 get_shader_program_VF(const std::string& path_vert, const std::string& path_frag) {
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
u32 get_shader_program_VGF(const std::string& path_vert, const std::string& path_geom,  const std::string& path_frag) {
	u32 vertex_shader = get_shader(path_vert, GL_VERTEX_SHADER);
	u32 geometry_shader = get_shader(path_geom, GL_GEOMETRY_SHADER);
	u32 fragment_shader = get_shader(path_frag, GL_FRAGMENT_SHADER);
	u32 shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, geometry_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
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

	return texture;
}
void set_uniform_texture(u32 location , u32 texture_id) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(location, 0);
}
void set_uniform(u32 location , Texture texture) {
	set_uniform_texture(location, texture.id);
}
struct Point { vec3 pos; vec2 uv = {0.f, 0.f}; };
struct Mesh /*Point*/ {
	u32 VBO, VAO, EBO, indices_count, points_size;
	Mesh(Point* points, u32 points_count, u32* indices, u32 indices_count) {
		points_size = points_count * sizeof(Point);
		this->indices_count = indices_count;
 		// indices_size = indices_count * sizeof(u32);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, points_size, points, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, pos));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, uv));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(Point), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
	~Mesh() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
};
//TODO fix utils.h/cpp for glm::vecN
struct Particle { vec3 pos; };
struct Particle_Cloud /*Particle*/ {
	// same as Mesh, but no EBO and different draw()
	u32 VBO, VAO, particles_count;
	Particle_Cloud(Particle* particles, u32 particles_count) {
		this->particles_count = particles_count;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, particles_count * sizeof(Particle), particles, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, pos));
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}
	~Particle_Cloud() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);	
	}
};
void draw(const Mesh& mesh) {
	glBindVertexArray(mesh.VAO);
	glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void draw(const Particle_Cloud& particle_cloud) {
	glBindVertexArray(particle_cloud.VAO);
	glDrawArrays(GL_POINTS, 0, particle_cloud.particles_count);
	// glDrawArraysInstanced(GL_POINTS, 0, particle_cloud.particles_count);
	glBindVertexArray(0);
}
struct Camera {
	glm::mat4 translation{1.f};
	glm::mat4 rotation{1.f};
};
Camera main_camera;
bool pressed_keys[1024] = {false};
void move_camera(float delta_time) {
	GLfloat cam_speed = 2.f, rot_speed = 2.0f;
	glm::vec4 direction = {0.f, 0.f, 0.f, 0.f};
	float angle = 0.f;
	if(pressed_keys[GLFW_KEY_W]) direction.z -= 1.f;
	if(pressed_keys[GLFW_KEY_S]) direction.z += 1.f;
	if(pressed_keys[GLFW_KEY_A]) direction.x -= 1.f;
	if(pressed_keys[GLFW_KEY_D]) direction.x += 1.f;	
	if(pressed_keys[GLFW_KEY_SPACE]) direction.y += 1.f;	
	if(pressed_keys[GLFW_KEY_LEFT_SHIFT]) direction.y -= 1.f;	
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
		if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
	}
}
struct I_Triangle { u32 a, b, c; };
struct Mesh_2D {
	std::vector<vec2> vertices;
	std::vector<I_Triangle> triangles;
};
// directly to Mesh_raw
Mesh load_mesh(const Mesh_2D& mesh_2d) {
	u32 V = mesh_2d.vertices.size();
	std::vector<Point> vertices(V);
	for(u32 i = 0; i < V; i++) {
		vec2 v = mesh_2d.vertices[i];
		vertices[i] = {{v.x, v.y, 0.f}};
	}
	Mesh mesh(&(vertices[0]), V, (u32*)&(mesh_2d.triangles[0]), mesh_2d.triangles.size() * 3);
	return mesh;
}
struct Value_Map {
	std::vector<float> values;
	u32 X, Y;

	Value_Map(std::function<float(float, float)> f, u32 X = 10, u32 Y = 10) {
		this->X = X, this->Y = Y;
		if(X < 2 || Y < 2) {
			printf("too small value map\n");
			exit(-1);
		}
		values.resize(X * Y);
		for(u32 y = 0, i = 0; y < Y; y++) {
			for(u32 x = 0; x < X; x++, i++) {
				values[i] = f((float)x / (X - 1), (float)y / (Y - 1));
				putchar(values[i] > 0 ? '+' : '-');
			}
			putchar('\n');
		}
		putchar('\n');
	}	
	float at(u32 x, u32 y) const {
		return values[x + y * X];
	}
};
Mesh_2D make_mesh_2d(const Value_Map& value_map) {
	u32 X = value_map.X, Y = value_map.Y;
	u32 S_nodes = X * Y, S_hor = Y * (X - 1), S_ver = (Y - 1) * X, S_total = S_nodes + S_hor + S_ver;
	Mesh_2D result;
	result.vertices.resize(S_total);

	// nodes
	for(u32 y = 0, i = 0; y < Y; y++) {
		for(u32 x = 0; x < X; x++, i++) {
			float xf = (float)x / (X - 1), yf = (float)y / (Y - 1);
			result.vertices[i] = {xf, yf};
		}
	}
	// hor
	for(u32 y = 0, i = 0; y < Y; y++) {
		for(u32 x = 0; x < X - 1; x++, i++) {
			float xf = (float)x / (X - 1), yf = (float)y / (Y - 1);
			float h1 = value_map.at(x, y), h2 = value_map.at(x + 1, y);
			float t = h1 / (h1 - h2);
			result.vertices[S_nodes + i] = {xf + t / (X - 1), yf};
		}
	}
	// ver
	for(u32 y = 0, i = 0; y < Y - 1; y++) {
		for(u32 x = 0; x < X; x++, i++) {
			float xf = (float)x / (X - 1), yf = (float)y / (Y - 1);
			float h1 = value_map.at(x, y), h2 = value_map.at(x, y + 1);
			float t = h1 / (h1 - h2);
			result.vertices[S_nodes + S_hor + i] = {xf, yf + t / (Y - 1)};
		}
	}
	auto add_triangles = [&result] (u32 a, u32 b, u32 c) -> void {
		result.triangles.push_back({a, b, c});
	};
	for(u32 y = 0, i = 0; y < Y - 1; y++) {
		for(u32 x = 0; x < X - 1; x++, i++) {
			u8 type =
				(value_map.at(x, y) > 0) |
				((value_map.at(x + 1, y) > 0) << 1) |
				((value_map.at(x, y + 1) > 0) << 2) |
				((value_map.at(x + 1, y + 1) > 0) << 3);
			u32 tl = x + y * X, tr = x + 1 + y * X, bl = x + (y + 1) * X, br = x + 1 + (y + 1) * X;
			u32 t = S_nodes + x + y * (X - 1), b = S_nodes + x + (y + 1) * (X - 1);
			u32 l = S_nodes + S_hor + x + y * X, r = S_nodes + S_hor + x + 1 + y * X;
			printf("%2d ", type);
			// add_triangles(tl, tr, br);
			switch(type) {
				case  0: break;
				case  1: add_triangles(tl, t, l); break;
				case  2: add_triangles(t, tr, r); break;
				case  3: add_triangles(tl, tr, r), add_triangles(tl, r, l); break;
				case  4: add_triangles(bl, l, b); break;
				case  5: add_triangles(tl, t, b), add_triangles(tl, b, bl); break;
				case  6: add_triangles(bl, l, b), add_triangles(t, tr, r); break;
				case  7: add_triangles(tl, tr, r), add_triangles(tl, r, b), add_triangles(tl, b, bl); break;
				case  8: add_triangles(r, br, b); break;
				case  9: add_triangles(r, br, b), add_triangles(l, tl, t); break;
				case 10: add_triangles(t, tr, br), add_triangles(t, br, b); break;
				case 11: add_triangles(tl, tr, br), add_triangles(tl, br, b), add_triangles(tl, b, l); break;
				case 12: add_triangles(l, r, br), add_triangles(l, br, bl); break;
				case 13: add_triangles(tl, t, r), add_triangles(tl, r, br), add_triangles(tl, br, bl); break;
				case 14: add_triangles(t, tr, br), add_triangles(t, br, bl), add_triangles(t, bl, l); break;
				case 15: add_triangles(tl, tr, br), add_triangles(tl, br, bl); break;
				default: printf("can't draw type #%d\n", type);
				// default: printf("impossible state\n"), exit(-1); break;
			}
		}
		putchar('\n');
	}
	putchar('\n');
	return result;
};
void clear() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
struct Frame_Buffer {
	u32 FBO, RBO, color_texture_id, depth_texture_id;
	Frame_Buffer() {
		glGenFramebuffersEXT(1, &FBO);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
			//TODO change to parameters
			u32 tex_x = 800, tex_y = 600;
			//color texture
			glGenTextures(1, &color_texture_id);
			glBindTexture(GL_TEXTURE_2D, color_texture_id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_x, tex_y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id, 0);

			glGenTextures(1, &depth_texture_id);
			glBindTexture(GL_TEXTURE_2D, depth_texture_id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, tex_x, tex_y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
			// glFramebufferTexture2D(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);

			glGenRenderbuffers(1, &RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, RBO);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, tex_x, tex_y);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

			if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				printf("ERROR: Framebuffer is not complete!");
			}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);  	
	}
	~Frame_Buffer() {
		glDeleteFramebuffers(1, &FBO);
	}
};

int main() {
	Window main_window;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	glfwSetKeyCallback(main_window.window, key_callback);

	// const u32 PARTICLE_COUNT = 10000;
	// std::vector<Particle> particles(PARTICLE_COUNT);
	// for(u32 i = 0; i < PARTICLE_COUNT; i++) particles[i] = {vec3{randf(), randf(), randf()} * 2.f - 1.f};

	Point cube_points[] = {
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

		{{ 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

		{{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}}
	};
	u32 cube_ids[36];
	for(u32 i = 0; i < 6; i++) {
		cube_ids[i*6  ] = i*4+0; 
		cube_ids[i*6+1] = i*4+1; 
		cube_ids[i*6+2] = i*4+2; 
		cube_ids[i*6+3] = i*4+0; 
		cube_ids[i*6+4] = i*4+2; 
		cube_ids[i*6+5] = i*4+3; 
	}	
	Mesh cube_mesh(cube_points, 24, cube_ids, 36);
	Point quad_points[] = {
		{{-0.9f, -0.9f,  0.0f},  {0.0f, 0.0f}},
		{{ 0.9f, -0.9f,  0.0f},  {1.0f, 0.0f}},
		{{ 0.9f,  0.9f,  0.0f},  {1.0f, 1.0f}},
		{{-0.9f,  0.9f,  0.0f},  {0.0f, 1.0f}},
	};
	u32 quad_ids[] = {0, 1, 2, 0, 2, 3};
	Mesh quad_mesh(quad_points, 4, quad_ids, 6);

	// Particle_Cloud particle_cloud(&(particles[0]), PARTICLE_COUNT);

	// u32 particle_shader = get_shader_program_VGF("particle.vert", "particle.geom", "particle.frag");
	// u32 uloc_transform_particle = glGetUniformLocation(particle_shader, "u_transform");

	u32 cube_shader = get_shader_program_VF("res/cube.vert", "res/cube.frag");
	u32 uloc_transform_cube = glGetUniformLocation(cube_shader, "u_transform");
	u32 uloc_tex_cube = glGetUniformLocation(cube_shader, "u_tex");

	u32 screen_shader = get_shader_program_VF("res/screen.vert", "res/screen.frag");
	u32 uloc_tex0_screen = glGetUniformLocation(screen_shader, "u_tex0"); 
	u32 uloc_tex1_screen = glGetUniformLocation(screen_shader, "u_tex1"); 

	Texture texture = load_texture("res/chio_rio.png");

	Frame_Buffer frame_buffer;

	float prev_time = glfwGetTime();	

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(delta_time); prev_time = new_time;
		glfwPollEvents();

		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.FBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer.depth_texture_id, 0);
			clear();
			{
				glm::mat4 projection(1.f), scale(1.f);
				scale = glm::scale(scale, glm::vec3(1.7f));
				projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
				glm::mat4 transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation) * scale;

				glUseProgram(cube_shader);
				set_uniform(uloc_tex_cube, texture);
				glUniformMatrix4fv(uloc_transform_cube, 1, GL_FALSE, glm::value_ptr(transform));

				glEnable(GL_DEPTH_TEST);
				draw(cube_mesh);	
				glDisable(GL_DEPTH_TEST);
			}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		clear();
		{
			glUseProgram(screen_shader);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, frame_buffer.color_texture_id);
			glUniform1i(uloc_tex0_screen, 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, frame_buffer.depth_texture_id);
			glUniform1i(uloc_tex1_screen, 1);

			draw(quad_mesh);
		}

		// glDisable(GL_DEPTH_TEST);
		// set_uniform_texture(uloc_tex_screen, frame_buffer.depth_texture_id);

		// draw(quad_mesh);

		// {
		// 	glm::mat4 projection(1.f);
		// 	projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
		// 	glm::mat4 transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation);

		// 	glDepthMask(GL_FALSE);
		// 	glEnable(GL_BLEND);
		// 	glBlendFunc(GL_ONE, GL_ONE);

		// 	glUseProgram(particle_shader);
		// 	glUniformMatrix4fv(uloc_transform_particle, 1, GL_FALSE, glm::value_ptr(transform));

		// 	draw(particle_cloud);

		// 	glDisable(GL_BLEND);
		// 	glDepthMask(GL_TRUE);
		// }
		glfwSwapBuffers(main_window.window);		
	}
	return 0;
}
int _main() {
	Window main_window;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	glfwSetKeyCallback(main_window.window, key_callback);

	Value_Map value_map([] (float x, float y) -> float { 
		return .15f - (x - .5f) * (x - .5f) - (y - .5f) * (y - .5f); 
	}, 100, 100);
	Mesh marching_squares = load_mesh(make_mesh_2d(value_map));

	u32 shader_program = get_shader_program_VF("vertex.vert", "fragment.frag");
	u32 uloc_transform = glGetUniformLocation(shader_program, "u_transform");

	float prev_time = glfwGetTime();
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(delta_time); prev_time = new_time;
		glfwPollEvents();

		glm::mat4 view(1.f), projection(1.f), scale(1.f);
		scale = glm::scale(scale, glm::vec3(1.f));
		projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
		glm::mat4 model(1.f);
		// model = glm::translate(model, cube_positions[i]);
		float angle = 0.f;//(float)(2.f * pow(i + 2.f, .2f) * new_time);
		model = glm::rotate(model, angle, glm::vec3(1.f, .3f, .5f));
		glm::mat4 transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation) * model * scale;

		clear();
		glUseProgram(shader_program);
		glUniformMatrix4fv(uloc_transform, 1, GL_FALSE, glm::value_ptr(transform));

		draw(marching_squares);

		glfwSwapBuffers(main_window.window);		
	}
	return 0;
}