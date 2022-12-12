#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SOIL/SOIL.h>
#include <glm/gtc/type_ptr.hpp>
#include <cassert>

#include "utils.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "marching_squares.h"
#include "perlin.h"
#include "defer.h"
#include "texture.h"
#include "model.h"
#include "window.h"


bool pressed_keys[1024] = {false};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if(action == GLFW_PRESS) {
		pressed_keys[key] = true;
	} else if(action == GLFW_RELEASE) {
		pressed_keys[key] = false;
		if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if(key == GLFW_MOUSE_BUTTON_LEFT) printf("!!!\n");
}
void clear(vec3 rgb = {0.f, 0.f, 0.f}) {
	glClearColor(rgb.r, rgb.g, rgb.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
struct Frame_Buffer {
	u32 FBO, RBO, color_texture_id, depth_texture_id, width, height;
	Frame_Buffer(u32 _width, u32 _height) {
		width = _width, height = _height;
		glGenFramebuffersEXT(1, &FBO);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBO);
			//TODO change to parameters (types of textures) or make different framebuffer t
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
		glDeleteTextures(1, &depth_texture_id);
		glDeleteTextures(1, &color_texture_id);
		glDeleteRenderbuffers(1, &RBO);
		glDeleteFramebuffers(1, &FBO);
	}
};
void set_buffer(const Frame_Buffer* frame_buffer) {
	glViewport(0, 0, frame_buffer->width, frame_buffer->height);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);	
}
void set_window_buffer(const Window* window) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window->width, window->height);
}

vec3 fix_normal(vec3 n, vec3 v) {
	return normalize(n - v * dot(v, n) / dot(v, v));
}
void fix_path_normals(std::vector<Strip_Node>& strip) {
	u32 N = strip.size();
	for(u32 i = 1; i < N - 1; i++) {
		vec3 v = normalize(strip[i + 1].pos - strip[i].pos) + normalize(strip[i].pos - strip[i - 1].pos);
		strip[i].norm = fix_normal(strip[i].norm, v);
	}
	strip[0].norm = fix_normal(strip[0].norm, strip[1].pos - strip[0].pos);
	strip[N - 1].norm = fix_normal(strip[N - 1].norm, strip[N - 1].pos - strip[N - 2].pos);
}
void fix_cycle_normals(std::vector<Strip_Node>& strip) {
	u32 N = strip.size();
	for(u32 i = 0; i < N; i++) {
		vec3 v = normalize(strip[(i + 1) % N].pos - strip[i].pos) + normalize(strip[i].pos - strip[(i + N - 1) % N].pos);
		strip[i].norm = fix_normal(strip[i].norm, v);
	}
}
std::vector<Strip_Node> make_smoother_path(const std::vector<Strip_Node>& origin, float theta = 1.f / 3.f) {
	u32 N = origin.size();
	std::vector<Strip_Node> result(N * 2);

	result[0] = origin[0], result[N * 2 - 1] = origin[N - 1];
	for(u32 i = 0; i < N - 1; i++) {
		vec3 a = origin[i].pos, b = origin[i + 1].pos;
		vec3 an = origin[i].norm, bn = origin[i + 1].norm;
		result[i * 2 + 1].pos = lerp(a, b, theta);
		result[i * 2 + 1].norm = lerp(an, bn, theta);
		result[i * 2 + 2].pos = lerp(b, a, theta);
		result[i * 2 + 2].norm = lerp(bn, an, theta);
	}
	fix_path_normals(result);
	return result;
}
std::vector<Strip_Node> make_smoother_cycle(const std::vector<Strip_Node>& origin, float theta = 1.f / 3.f) {
	u32 N = origin.size();
	std::vector<Strip_Node> result(N * 2);

	for(u32 i = 0; i < N; i++) {
		vec3 a = origin[i].pos, b = origin[(i + 1) % N].pos;
		vec3 an = origin[i].norm, bn = origin[(i + 1) % N].norm;
		result[i * 2].pos = lerp(a, b, theta);
		result[i * 2].norm = lerp(an, bn, theta);
		result[i * 2 + 1].pos = lerp(b, a, theta);
		result[i * 2 + 1].norm = lerp(bn, an, theta);
	}
	fix_cycle_normals(result);
	return result;
}
std::vector<Strip_Node> generate_random_smooth_path(u32 count, u32* result_size_ptr) {
	u32 iterations = 5, result_size = count << iterations;
	*result_size_ptr = result_size;
	std::vector<Strip_Node> origin(count);

	for(u32 i = 0; i < count; i++) { origin[i] = {{randf(), randf(), (float)i / count}, rand_vec3_unit()}; }
	fix_path_normals(origin);
	for(u32 i = 0; i < iterations; i++) origin = make_smoother_path(origin);

	std::vector<Strip_Node> result(result_size + 2);
	result[0] = result[result_size + 1] = Strip_Node{.t = -1.f};
	for(u32 i = 0; i < result_size; i++) { result[i + 1] = origin[i]; }	
	
	return result;
}
std::vector<Strip_Node> generate_random_smooth_cycle(u32 count, u32* result_size_ptr) {
	// count = 4;

	u32 iterations = 5, result_size = count << iterations;
	*result_size_ptr = result_size + 1;

	std::vector<Strip_Node> origin(count);
	for(u32 i = 0; i < count; i++) { origin[i] = {rand_vec3() * 3.f, rand_vec3_unit()}; }

	// std::vector<Strip_Node> origin(4);
	// 	origin[0] = {{0.f, 0.f, 0.f}, {0.0f, 0.0f, 1.f}};
	// 	origin[1] = {{0.f, 1.f, 0.f}, {0.0f, 0.0f, 1.f}};
	// 	origin[2] = {{1.f, 1.f, 0.f}, {0.0f, 0.0f, 1.f}};
	// 	origin[3] = {{1.f, 0.f, 0.f}, {0.0f, 0.0f, 1.f}};

	fix_cycle_normals(origin);
	for(u32 i = 0; i < iterations; i++) origin = make_smoother_cycle(origin, .25f);

	std::vector<Strip_Node> result(result_size + 3);
	for(u32 i = 0; i < result_size; i++) { result[i] = origin[i]; }	
	result[result_size] = origin[0]; result[0].t = 1.f;
	result[result_size + 1] = origin[1]; 
	result[result_size + 2] = origin[2]; result[result_size + 2].t = 1.f;
	
	return result;
}

//TODO defer/~ glDeleteProgram  & shader class
int main() {
	Window main_window(1200, 800);
	glfwSetKeyCallback(main_window.window, key_callback);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

	// Model teapot("./res/cat/scene.gltf"); AC(teapot);
	Model teapot("./res/backpack/backpack.obj"); AC(teapot);
	// Model teapot("./res/6th_platonic_solid.obj");
	// Model teapot("./res/cube.obj");

	Shader shader_default = get_shader_program_VF("res/default.vert", "res/default.frag"); AC(shader_default)
		u32 uloc_teapot_trworld = glGetUniformLocation(shader_default.id, "u_trworld");
		u32 uloc_teapot_trscreen = glGetUniformLocation(shader_default.id, "u_trscreen");
		u32 uloc_teapot_view_pos = glGetUniformLocation(shader_default.id, "u_view_pos");
		u32 uloc_teapot_tex_diff = glGetUniformLocation(shader_default.id, "u_tex_diff");
		u32 uloc_teapot_tex_spec = glGetUniformLocation(shader_default.id, "u_tex_spec");
		u32 uloc_teapot_tex_norm = glGetUniformLocation(shader_default.id, "u_tex_norm");
		u32 uloc_teapot_time = glGetUniformLocation(shader_default.id, "u_time");
	Shader shader_normals = get_shader_program_VGF("res/default.vert", "res/normals.geom", "res/lamp.frag"); AC(shader_normals)
		u32 uloc_normals_trworld = glGetUniformLocation(shader_normals.id, "u_trworld");
		u32 uloc_normals_trscreen = glGetUniformLocation(shader_normals.id, "u_trscreen");

	float prev_time = glfwGetTime();	
	
	glEnable(GL_CULL_FACE);  
	glEnable(GL_DEPTH_TEST);  
	glEnable(GL_MULTISAMPLE);
	
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glm::mat4 screen_transform = get_transform(&main_camera, &main_window);

		glfwPollEvents();
		clear(vec3(1.f, 1.f, 1.f));
		{
			glm::mat4 world_transform(1.f);
			// world_transform = glm::scale(world_transform, vec3(.1f, .1f, .1f));
			// world_transform = glm::translate(world_transform, vec3(0.f, -8.f, -4.f));
			// world_transform = glm::rotate(world_transform, new_time * 2.f, vec3(0.f, 1.f, 0.f));

			glUseProgram(shader_default.id);
				glUniformMatrix4fv(uloc_teapot_trworld, 1, GL_FALSE, (GLfloat*)&world_transform);
				glUniformMatrix4fv(uloc_teapot_trscreen, 1, GL_FALSE, (GLfloat*)&screen_transform);
				vec3 eye = get_position(main_camera);
				glUniform3f(uloc_teapot_view_pos, eye.x, eye.y, eye.z);
				glUniform1f(uloc_teapot_time, new_time);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
				draw(teapot, uloc_teapot_tex_diff, uloc_teapot_tex_spec, uloc_teapot_tex_norm);

			// glUseProgram(shader_normals);
			// 	glUniformMatrix4fv(uloc_normals_trworld, 1, GL_FALSE, (GLfloat*)&world_transform);
			// 	glUniformMatrix4fv(uloc_normals_trscreen, 1, GL_FALSE, (GLfloat*)&screen_transform);

			// 	draw(teapot, uloc_teapot_tex_diff, uloc_teapot_tex_spec, uloc_teapot_tex_norm);
		}

		glfwSwapBuffers(main_window.window);		
	}
	return 0;
}

int main_2() {
	Window main_window(1200, 800);
	Frame_Buffer frame_buffer(2400, 1600);

	Camera main_camera;

	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	glfwSetKeyCallback(main_window.window, key_callback);

	Point_UV quad_points[] = {
		{{-1, -1,  0},  {0, 0}},
		{{ 1, -1,  0},  {1, 0}},
		{{ 1,  1,  0},  {1, 1}},
		{{-1,  1,  0},  {0, 1}},
	};
	u32 quad_ids[] = {0, 1, 2, 0, 2, 3};
	auto quad_mesh = make_mesh(link_mesh_uv, quad_points, 4, quad_ids, 6);

	const u32 PARTICLES_COUNT = 200;
	std::vector<Particle> particles(PARTICLES_COUNT);
	for(u32 i = 0; i < PARTICLES_COUNT; i++) {
		vec2 p2 = rand_vec2_unit();
		particles[i].pos = vec3{p2.x, p2.y, 0.f} + (rand_vec3() * .3f - .15f);
	}

	Mesh_Any particle_cloud = make_mesh(link_particles, &(particles[0]), PARTICLES_COUNT); AC(particle_cloud);

	Shader screen_shader = get_shader_program_VF("res/screen.vert", "res/screen.frag"); AC(screen_shader)
		u32 uloc_screen_factor_screen = glGetUniformLocation(screen_shader.id, "u_screen_factor");
		u32 uloc_tex0_screen = glGetUniformLocation(screen_shader.id, "u_tex0"); 
		u32 uloc_tex1_screen = glGetUniformLocation(screen_shader.id, "u_tex1"); 
	Shader particle_shader = get_shader_program_VGF("res/particle.vert", "res/particle.geom", "res/particle2.frag"); AC(particle_shader)
		u32 uloc_tex_particle = glGetUniformLocation(screen_shader.id, "u_tex"); 
		u32 uloc_transform_particle = glGetUniformLocation(particle_shader.id, "u_transform");	
		u32 uloc_ssize_particle = glGetUniformLocation(particle_shader.id, "u_screen_size");
		u32 uloc_time_particle = glGetUniformLocation(particle_shader.id, "u_time");
		u32 uloc_psize_particle = glGetUniformLocation(particle_shader.id, "u_particle_size");
	float prev_time = glfwGetTime();	

	glEnable(GL_MULTISAMPLE);

	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();

		glm::mat4 projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
		glm::mat4 global_transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation);

		set_buffer(&frame_buffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer.depth_texture_id, 0);
			clear(vec3{1.f, 1.f ,1.f});
			/* slices */ {
				// glm::mat4 local_transform(1.f);
				// local_transform = glm::scale(local_transform, glm::vec3(1.7f));
				// glm::mat4 transform = global_transform * local_transform;

				// glUseProgram(blob_shader);
				// 	glUniformMatrix4fv(uloc_blob_transform, 1, GL_FALSE, glm::value_ptr(transform));

				// glEnable(GL_DEPTH_TEST); DEFER(glDisable(GL_DEPTH_TEST);)
				// 	draw(blob_mesh);
			}
		set_window_buffer(&main_window);

		clear(vec3{1.f, 1.f ,1.f});
		/* screen */ {
			glUseProgram(screen_shader.id);
				set_uniform_texture(uloc_tex0_screen, frame_buffer.color_texture_id, 0);
				set_uniform_texture(uloc_tex1_screen, frame_buffer.depth_texture_id, 1);
				glUniform2f(uloc_screen_factor_screen, (float)main_window.width / frame_buffer.width, (float)main_window.height / frame_buffer.height);

			glDisable(GL_DEPTH_TEST);
				draw_uv(quad_mesh);
		}

		/* particles */ {
			glm::mat4 local_transform(1.f);
			local_transform = glm::scale(local_transform, glm::vec3(1.7f));
			glm::mat4 transform = global_transform * local_transform;


			glUseProgram(particle_shader.id);
				glUniformMatrix4fv(uloc_transform_particle, 1, GL_FALSE, glm::value_ptr(transform));
				glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, frame_buffer.depth_texture_id);
				glUniform1i(uloc_tex_particle, 0);
				glUniform2f(uloc_ssize_particle, (float)main_window.width, (float)main_window.height);
				glUniform1f(uloc_time_particle, new_time);
				glUniform1f(uloc_psize_particle, 150);

			glEnable(GL_DEPTH_TEST); 
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				draw_particles(particle_cloud);
		}
		glfwSwapBuffers(main_window.window);		
	}
	return 0;
}

int main_1() {
	Window main_window(1200, 800);
	glfwSetKeyCallback(main_window.window, key_callback);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

	vec3 zone_size = {1.f, 1.f, 1.f};
	u32 POINTS_COUNT;
	std::vector<Strip_Node> points = generate_random_smooth_cycle(30, &POINTS_COUNT);

	float total_length = 0.f;
	for(u32 i = 1, n = points.size(); i < POINTS_COUNT + 1; i++) {
		points[i].t = total_length;
		if(i != POINTS_COUNT) total_length += glm::length((points[i + 1].pos - points[i].pos) / zone_size);
	}	

	auto line_strip = make_mesh(link_line_strip, &(points[0]), POINTS_COUNT + 2);

	Shader shader_ls = get_shader_program_VGF("res/line_strip.vert", "res/line_strip.geom", "res/line_strip.frag"); AC(shader_ls)
		u32 uloc_ls_zs = glGetUniformLocation(shader_ls.id, "u_zone_size");
		u32 uloc_ls_pc = glGetUniformLocation(shader_ls.id, "u_point_count");
		u32 uloc_ls_length = glGetUniformLocation(shader_ls.id, "u_length");
		u32 uloc_ls_tex = glGetUniformLocation(shader_ls.id, "u_texture");
		u32 uloc_ls_ts = glGetUniformLocation(shader_ls.id, "u_tex_size");
		u32 uloc_ls_rad = glGetUniformLocation(shader_ls.id, "u_r");
		u32 uloc_ls_transform = glGetUniformLocation(shader_ls.id, "u_transform");
		u32 uloc_ls_time = glGetUniformLocation(shader_ls.id, "u_time");

	Texture texture = load_texture("res/chio_rio.png");

	float prev_time = glfwGetTime();	
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();
		clear();

		{
			glUseProgram(shader_ls.id);
				glUniform3f(uloc_ls_zs, zone_size.x, zone_size.y, zone_size.z);
				glUniform1i(uloc_ls_pc, POINTS_COUNT + 1);
				glUniform1f(uloc_ls_length, total_length);
				set_uniform(uloc_ls_tex, texture);
				glUniform2f(uloc_ls_ts, (float)texture.info.width, (float)texture.info.height);
				glUniform1f(uloc_ls_rad, .15f);
				glUniformMatrix4fv(uloc_ls_transform, 1, GL_FALSE, glm::value_ptr(get_transform(&main_camera, &main_window)));
				glUniform1f(uloc_ls_time, new_time);

			glEnable(GL_BLEND);
			glBlendEquation(GL_MAX);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				draw_line_strip(line_strip);
		}

		glfwSwapBuffers(main_window.window);		
	}
	return 0;
}

int main_0() {
	Window main_window(1200, 800);
	Frame_Buffer frame_buffer(2400, 1600);

	Camera main_camera;

	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	glfwSetKeyCallback(main_window.window, key_callback);

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

	auto cube_mesh = make_mesh(link_mesh_uv, cube_points, 24, cube_ids, 36);
	Point_UV quad_points[] = {
		{{-1, -1,  0},  {0, 0}},
		{{ 1, -1,  0},  {1, 0}},
		{{ 1,  1,  0},  {1, 1}},
		{{-1,  1,  0},  {0, 1}},
	};
	u32 quad_ids[] = {0, 1, 2, 0, 2, 3};
	auto quad_mesh = make_mesh(link_mesh_uv, quad_points, 4, quad_ids, 6);

	PerlinNoise generator;
	auto [blob_mesh, line_set] = make_layers_mesh([&generator] (float x, float y, float z) { 
		x = x * 2 - 1, y = y * 2 - 1, z = z * 2 - 1;
		float r = sqrtf(x * x + y * y) + fabsf(z);
		float h0 = .9 - r;
		float dh = generator.noise((x + 5) * 10, (y + 5) * 10, (z + 5) * 10) * .2;
		return h0 + dh;
	}, 60, 60, 12);
	AC(blob_mesh);

	const u32 PARTICLES_COUNT = 5000;
	std::vector<Particle> particles = spawn_particles(&line_set, PARTICLES_COUNT, 0.015);
	Mesh_Any particle_cloud = make_mesh(link_particles, &(particles[0]), PARTICLES_COUNT); AC(particle_cloud);

	//shaders
	Shader blob_shader = get_shader_program_VF("res/cube.vert", "res/cube.frag"); AC(blob_shader)
		u32 uloc_blob_transform = glGetUniformLocation(blob_shader.id, "u_transform");
	Shader screen_shader = get_shader_program_VF("res/screen.vert", "res/screen.frag"); AC(screen_shader)
		u32 uloc_screen_factor_screen = glGetUniformLocation(screen_shader.id, "u_screen_factor");
		u32 uloc_tex0_screen = glGetUniformLocation(screen_shader.id, "u_tex0"); 
		u32 uloc_tex1_screen = glGetUniformLocation(screen_shader.id, "u_tex1");
	Shader particle_shader = get_shader_program_VGF("res/particle.vert", "res/particle.geom", "res/particle.frag"); AC(particle_shader)
		u32 uloc_tex_particle = glGetUniformLocation(screen_shader.id, "u_tex"); 
		u32 uloc_transform_particle = glGetUniformLocation(particle_shader.id, "u_transform");	
		u32 uloc_ssize_particle = glGetUniformLocation(particle_shader.id, "u_screen_size");
		u32 uloc_time_particle = glGetUniformLocation(particle_shader.id, "u_time");
		u32 uloc_psize_particle = glGetUniformLocation(particle_shader.id, "u_particle_size");

	float prev_time = glfwGetTime();	

	glEnable(GL_MULTISAMPLE);

	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();

		glm::mat4 projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
		glm::mat4 global_transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation);

		set_buffer(&frame_buffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer.depth_texture_id, 0);
			clear();
			/* slices */ {
				glm::mat4 local_transform(1.f);
				local_transform = glm::scale(local_transform, glm::vec3(1.7f));
				glm::mat4 transform = global_transform * local_transform;

				glUseProgram(blob_shader.id);
					glUniformMatrix4fv(uloc_blob_transform, 1, GL_FALSE, glm::value_ptr(transform));

				glEnable(GL_DEPTH_TEST); DEFER(glDisable(GL_DEPTH_TEST);)
					draw_uv(blob_mesh);
			}
		set_window_buffer(&main_window);

		clear();
		/* screen */ {
			glUseProgram(screen_shader.id);
				set_uniform_texture(uloc_tex0_screen, frame_buffer.color_texture_id, 0);
				set_uniform_texture(uloc_tex1_screen, frame_buffer.depth_texture_id, 1);
				glUniform2f(uloc_screen_factor_screen, (float)main_window.width / frame_buffer.width, (float)main_window.height / frame_buffer.height);

			draw_uv(quad_mesh);
		}

		/* particles */ {
			glm::mat4 local_transform(1.f);
			local_transform = glm::scale(local_transform, glm::vec3(1.7f));
			glm::mat4 transform = global_transform * local_transform;


			glUseProgram(particle_shader.id);
				glUniformMatrix4fv(uloc_transform_particle, 1, GL_FALSE, glm::value_ptr(transform));
				glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, frame_buffer.depth_texture_id);
				glUniform1i(uloc_tex_particle, 0);
				glUniform2f(uloc_ssize_particle, (float)main_window.width, (float)main_window.height);
				glUniform1f(uloc_time_particle, new_time);
				glUniform1f(uloc_psize_particle, 20);

			glDepthMask(GL_FALSE); DEFER(glDepthMask(GL_TRUE);)
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				draw_particles(particle_cloud);
		}
		glfwSwapBuffers(main_window.window);		
	}
	return 0;
}
