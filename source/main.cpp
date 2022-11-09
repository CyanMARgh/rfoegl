#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SOIL/SOIL.h>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "marching_squares.h"
#include "perlin.h"

struct Window {
	GLFWwindow* window;
	int width, height;
	
	Window(u32 _width, u32 _height) {
		width = _width, height = _height;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
void set_uniform_texture(u32 location, u32 texture_id, u32 delta = 0) {
	glActiveTexture(GL_TEXTURE0 + delta);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(location, delta);
}
void set_uniform(u32 location , Texture texture, u32 delta = 0) {
	set_uniform_texture(location, texture.id, delta);
}
void draw(const Mesh_UV& mesh_UV) {
	glBindVertexArray(mesh_UV.VAO);
	glDrawElements(GL_TRIANGLES, mesh_UV.indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void draw(const Particle_Cloud& particle_cloud) {
	glBindVertexArray(particle_cloud.VAO);
	glDrawArrays(GL_POINTS, 0, particle_cloud.particles_count);
	// glDrawArraysInstanced(GL_POINTS, 0, particle_cloud.particles_count);
	glBindVertexArray(0);
}

bool pressed_keys[1024] = {false};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if(action == GLFW_PRESS) {
		pressed_keys[key] = true;
	} else if(action == GLFW_RELEASE) {
		pressed_keys[key] = false;
		if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
	}
}
void clear() {
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
struct Frame_Buffer {
	u32 FBO, RBO, color_texture_id, depth_texture_id, width, height;
	Frame_Buffer(u32 _width, u32 _height) {
		width = _width, height = _height;
		glGenFramebuffersEXT(1, &FBO);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
			//TODO change to parameters
			//color texture
			glGenTextures(1, &color_texture_id);
			glBindTexture(GL_TEXTURE_2D, color_texture_id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id, 0);

			glGenTextures(1, &depth_texture_id);
			glBindTexture(GL_TEXTURE_2D, depth_texture_id);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
			// glFramebufferTexture2D(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);

			glGenRenderbuffers(1, &RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, RBO);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
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
	Window main_window(1200, 800);
	Frame_Buffer frame_buffer(2400, 1600);

	Camera main_camera;

	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	glfwSetKeyCallback(main_window.window, key_callback);

	//models
	// const u32 PARTICLE_COUNT = 10000;
	// std::vector<Particle> particles(PARTICLE_COUNT);
	// for(u32 i = 0; i < PARTICLE_COUNT; i++) particles[i] = {rand_vec3() * 2.f - 1.f};
	// Particle_Cloud particle_cloud(&(particles[0]), PARTICLE_COUNT);
	Point_UV cube_points[] = {
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
	Mesh_UV cube_mesh(cube_points, 24, cube_ids, 36);
	Point_UV quad_points[] = {
		{{-1, -1,  0},  {0, 0}},
		{{ 1, -1,  0},  {1, 0}},
		{{ 1,  1,  0},  {1, 1}},
		{{-1,  1,  0},  {0, 1}},
	};
	u32 quad_ids[] = {0, 1, 2, 0, 2, 3};
	Mesh_UV quad_mesh(quad_points, 4, quad_ids, 6);

	PerlinNoise generator;
	auto [blob_mesh, line_set] = make_layers_mesh([&generator] (float x, float y, float z) { 
		x = x * 2 - 1, y = y * 2 - 1, z = z * 2 - 1;
		float r = sqrtf(x * x + y * y) + fabsf(z);
		float h0 = .9 - r;
		float dh = generator.noise((x + 5) * 10, (y + 5) * 10, (z + 5) * 10) * .2;
		return h0 + dh;
	}, 60, 60, 12);

	const u32 PARTICLES_COUNT = 20000;
	std::vector<Particle> particles = spawn_particles(&line_set, PARTICLES_COUNT, 0.015);
	Particle_Cloud particle_cloud(&(particles[0]), PARTICLES_COUNT);

	//shaders
	u32 blob_shader = get_shader_program_VF("res/cube.vert", "res/cube.frag");
		u32 uloc_blob_transform = glGetUniformLocation(blob_shader, "u_transform");
	// u32 cube_shader = get_shader_program_VF("res/cube.vert", "res/cube.frag");
	// 	u32 uloc_transform_cube = glGetUniformLocation(cube_shader, "u_transform");
	// 	u32 uloc_tex_cube = glGetUniformLocation(cube_shader, "u_tex");
	u32 screen_shader = get_shader_program_VF("res/screen.vert", "res/screen.frag");
		u32 uloc_screen_factor_screen = glGetUniformLocation(screen_shader, "u_screen_factor");
		u32 uloc_tex0_screen = glGetUniformLocation(screen_shader, "u_tex0"); 
		u32 uloc_tex1_screen = glGetUniformLocation(screen_shader, "u_tex1"); 
	u32 particle_shader = get_shader_program_VGF("res/particle.vert", "res/particle.geom", "res/particle.frag");
		u32 uloc_tex_particle = glGetUniformLocation(screen_shader, "u_tex"); 
		u32 uloc_transform_particle = glGetUniformLocation(particle_shader, "u_transform");	
		u32 uloc_size_particle = glGetUniformLocation(particle_shader, "u_screen_size");
		u32 uloc_time_particle = glGetUniformLocation(particle_shader, "u_time");

	//textures
	Texture texture = load_texture("res/chio_rio.png");

	float prev_time = glfwGetTime();	

	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);

	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();

		glm::mat4 projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
		glm::mat4 global_transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation);

		// clear();

		// glUseProgram(blob_shader);
		// glUniformMatrix4fv(uloc_blob_transform, 1, GL_FALSE, glm::value_ptr(global_transform));

		// glEnable(GL_DEPTH_TEST);
		// draw(blob_mesh);
		// glDisable(GL_DEPTH_TEST);

		glViewport(0, 0, frame_buffer.width, frame_buffer.height);
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.FBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer.depth_texture_id, 0);
			clear();
			/* slices */ {
				glm::mat4 local_transform(1.f);
				local_transform = glm::scale(local_transform, glm::vec3(1.7f));
				glm::mat4 transform = global_transform * local_transform;

				glUseProgram(blob_shader);
				glUniformMatrix4fv(uloc_blob_transform, 1, GL_FALSE, glm::value_ptr(transform));

				glEnable(GL_DEPTH_TEST);
				draw(blob_mesh);	
				glDisable(GL_DEPTH_TEST);
			}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, main_window.width, main_window.height);
		clear();

		/* screen */ {
			glUseProgram(screen_shader);

			set_uniform_texture(uloc_tex0_screen, frame_buffer.color_texture_id, 0);
			set_uniform_texture(uloc_tex1_screen, frame_buffer.depth_texture_id, 1);
			glUniform2f(uloc_screen_factor_screen, (float)main_window.width / frame_buffer.width, (float)main_window.height / frame_buffer.height);

			draw(quad_mesh);
		}

		/* particles */ {
			glm::mat4 local_transform(1.f);
			local_transform = glm::scale(local_transform, glm::vec3(1.7f));
			glm::mat4 transform = global_transform * local_transform;

			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(particle_shader);
			glUniformMatrix4fv(uloc_transform_particle, 1, GL_FALSE, glm::value_ptr(transform));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, frame_buffer.depth_texture_id);
			glUniform1i(uloc_tex_particle, 0);
			glUniform2f(uloc_size_particle, (float)main_window.width, (float)main_window.height);
			glUniform1f(uloc_time_particle, new_time);

			glEnable(GL_DEPTH_TEST);
			draw(particle_cloud);
			glDisable(GL_DEPTH_TEST);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
		}
		glfwSwapBuffers(main_window.window);		
	}
	return 0;
}
