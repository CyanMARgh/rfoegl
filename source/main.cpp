#include "demo_0.h"

// #define GLEW_STATIC
// #include <GL/glew.h>
// #include <GLFW/glfw3.h>

// #include <SOIL/SOIL.h>
// #include <glm/gtc/type_ptr.hpp>
// #include <cassert>

// #include "utils.h"
// #include "camera.h"
// #include "mesh.h"
// #include "shader.h"
// #include "marching_squares.h"
// #include "perlin.h"
// #include "defer.h"
// #include "texture.h"
// #include "model.h"
// #include "framebuffer.h"

// int main_4() {
// 	Window main_window(1200, 800);
// 	Frame_Buffer frame_buffer(1200, 800);

// 	Camera main_camera;

// 	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
// 	glfwSetKeyCallback(main_window.window, key_callback);

// 	Point_UV cube_points[] = {
// 		{{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
// 		{{-0.5f,  0.5f, -0.5f},  {1.0f, 0.0f}},
// 		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
// 		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

// 		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
// 		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
// 		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}},
// 		{{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f}},

// 		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
// 		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
// 		{{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
// 		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

// 		{{ 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f}},
// 		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
// 		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 1.0f}},
// 		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

// 		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
// 		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
// 		{{-0.5f, -0.5f,  0.5f},  {1.0f, 1.0f}},
// 		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

// 		{{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f}},
// 		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
// 		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
// 		{{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}}
// 	};
// 	u32 cube_ids[36];
// 	for(u32 i = 0; i < 6; i++) {
// 		cube_ids[i*6  ] = i*4+0; 
// 		cube_ids[i*6+1] = i*4+1; 
// 		cube_ids[i*6+2] = i*4+2; 
// 		cube_ids[i*6+3] = i*4+0; 
// 		cube_ids[i*6+4] = i*4+2; 
// 		cube_ids[i*6+5] = i*4+3; 
// 	}	


// 	u32 ac_buf;
// 	glGenBuffers(1, &ac_buf); DEFER(glDeleteBuffers(1, &ac_buf));
// 	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ac_buf);
// 	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(u32), NULL, GL_DYNAMIC_DRAW);
// 	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);


// 	auto cube_mesh = make_mesh(link_mesh_uv, cube_points, 24, cube_ids, 36);
// 	Point_UV quad_points[] = {
// 		{{-1, -1,  0},  {0, 0}},
// 		{{ 1, -1,  0},  {1, 0}},
// 		{{ 1,  1,  0},  {1, 1}},
// 		{{-1,  1,  0},  {0, 1}},
// 	};
// 	u32 quad_ids[] = {0, 1, 2, 0, 2, 3};
// 	auto quad_mesh = make_mesh(link_mesh_uv, quad_points, 4, quad_ids, 6);

// 	//shaders
// 	Shader cube_shader = get_shader_program_VF("res/lamp.vert", "res/lamp.frag"); AC(cube_shader)
// 		u32 uloc_cube_transform = glGetUniformLocation(cube_shader.id, "u_transform");
// 		u32 uloc_cube_color = glGetUniformLocation(cube_shader.id, "u_color");
// 	Shader screen_shader = get_shader_program_VF("res/screen.vert", "res/screen.frag"); AC(screen_shader)
// 		u32 uloc_screen_factor_screen = glGetUniformLocation(screen_shader.id, "u_screen_factor");
// 		u32 uloc_tex0_screen = glGetUniformLocation(screen_shader.id, "u_tex0"); 
// 		u32 uloc_tex1_screen = glGetUniformLocation(screen_shader.id, "u_tex1");

// 	float prev_time = glfwGetTime();	

// 	glEnable(GL_MULTISAMPLE);

// 	while(!glfwWindowShouldClose(main_window.window)) {
// 		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
// 		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
// 		glfwPollEvents();

// 		glm::mat4 projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
// 		glm::mat4 global_transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation);

// 		clear();
// 		/* cube */ {
// 			glm::mat4 local_transform(1.f);
// 			local_transform = glm::scale(local_transform, glm::vec3(1.7f));
// 			glm::mat4 transform = global_transform * local_transform;

// 			glUseProgram(cube_shader.id);
// 				glUniformMatrix4fv(uloc_cube_transform, 1, GL_FALSE, glm::value_ptr(transform));
// 				glUniform3f(uloc_cube_color, 0.f, 1.f, 1.f);

// 			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, ac_buf);

// 			glEnable(GL_DEPTH_TEST); DEFER(glDisable(GL_DEPTH_TEST);)
// 				draw_uv(cube_mesh);
// 		}
// 		glfwSwapBuffers(main_window.window);
// 	}
// 	return 0;
// }
// int main_3() {
// 	Window main_window(1200, 800);
// 	glfwSetKeyCallback(main_window.window, key_callback);

// 	Camera main_camera;
// 	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

// 	// Model teapot("./res/cat/scene.gltf"); AC(teapot);
// 	Model teapot("./res/backpack/backpack.obj"); AC(teapot);
// 	// Model teapot("./res/6th_platonic_solid.obj");
// 	// Model teapot("./res/cube.obj");

// 	Shader shader_default = get_shader_program_VF("res/default.vert", "res/default.frag"); AC(shader_default)
// 		u32 uloc_teapot_trworld = glGetUniformLocation(shader_default.id, "u_trworld");
// 		u32 uloc_teapot_trscreen = glGetUniformLocation(shader_default.id, "u_trscreen");
// 		u32 uloc_teapot_view_pos = glGetUniformLocation(shader_default.id, "u_view_pos");
// 		u32 uloc_teapot_tex_diff = glGetUniformLocation(shader_default.id, "u_tex_diff");
// 		u32 uloc_teapot_tex_spec = glGetUniformLocation(shader_default.id, "u_tex_spec");
// 		u32 uloc_teapot_tex_norm = glGetUniformLocation(shader_default.id, "u_tex_norm");
// 		u32 uloc_teapot_time = glGetUniformLocation(shader_default.id, "u_time");
// 	Shader shader_normals = get_shader_program_VGF("res/default.vert", "res/normals.geom", "res/lamp.frag"); AC(shader_normals)
// 		u32 uloc_normals_trworld = glGetUniformLocation(shader_normals.id, "u_trworld");
// 		u32 uloc_normals_trscreen = glGetUniformLocation(shader_normals.id, "u_trscreen");

// 	float prev_time = glfwGetTime();	
	
// 	glEnable(GL_CULL_FACE);  
// 	glEnable(GL_DEPTH_TEST);  
// 	glEnable(GL_MULTISAMPLE);
	
// 	while(!glfwWindowShouldClose(main_window.window)) {
// 		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
// 		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
// 		glm::mat4 screen_transform = get_transform(&main_camera, &main_window);

// 		glfwPollEvents();
// 		clear(vec3(1.f, 1.f, 1.f));
// 		{
// 			glm::mat4 world_transform(1.f);
// 			// world_transform = glm::scale(world_transform, vec3(.1f, .1f, .1f));
// 			// world_transform = glm::translate(world_transform, vec3(0.f, -8.f, -4.f));
// 			// world_transform = glm::rotate(world_transform, new_time * 2.f, vec3(0.f, 1.f, 0.f));

// 			glUseProgram(shader_default.id);
// 				glUniformMatrix4fv(uloc_teapot_trworld, 1, GL_FALSE, (GLfloat*)&world_transform);
// 				glUniformMatrix4fv(uloc_teapot_trscreen, 1, GL_FALSE, (GLfloat*)&screen_transform);
// 				vec3 eye = get_position(main_camera);
// 				glUniform3f(uloc_teapot_view_pos, eye.x, eye.y, eye.z);
// 				glUniform1f(uloc_teapot_time, new_time);

// 			glEnable(GL_BLEND);
// 			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
// 				draw(teapot, uloc_teapot_tex_diff, uloc_teapot_tex_spec, uloc_teapot_tex_norm);

// 			// glUseProgram(shader_normals);
// 			// 	glUniformMatrix4fv(uloc_normals_trworld, 1, GL_FALSE, (GLfloat*)&world_transform);
// 			// 	glUniformMatrix4fv(uloc_normals_trscreen, 1, GL_FALSE, (GLfloat*)&screen_transform);

// 			// 	draw(teapot, uloc_teapot_tex_diff, uloc_teapot_tex_spec, uloc_teapot_tex_norm);
// 		}

// 		glfwSwapBuffers(main_window.window);		
// 	}
// 	return 0;
// }
// int main_2() {
// 	Window main_window(1200, 800);
// 	Frame_Buffer frame_buffer(2400, 1600);

// 	Camera main_camera;

// 	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
// 	glfwSetKeyCallback(main_window.window, key_callback);

// 	Point_UV quad_points[] = {
// 		{{-1, -1,  0},  {0, 0}},
// 		{{ 1, -1,  0},  {1, 0}},
// 		{{ 1,  1,  0},  {1, 1}},
// 		{{-1,  1,  0},  {0, 1}},
// 	};
// 	u32 quad_ids[] = {0, 1, 2, 0, 2, 3};
// 	auto quad_mesh = make_mesh(link_mesh_uv, quad_points, 4, quad_ids, 6);

// 	const u32 PARTICLES_COUNT = 200;
// 	std::vector<Particle> particles(PARTICLES_COUNT);
// 	for(u32 i = 0; i < PARTICLES_COUNT; i++) {
// 		vec2 p2 = rand_vec2_unit();
// 		particles[i].pos = vec3{p2.x, p2.y, 0.f} + (rand_vec3() * .3f - .15f);
// 	}

// 	Mesh_Any particle_cloud = make_mesh(link_particles, &(particles[0]), PARTICLES_COUNT); AC(particle_cloud);

// 	Shader screen_shader = get_shader_program_VF("res/screen.vert", "res/screen.frag"); AC(screen_shader)
// 		u32 uloc_screen_factor_screen = glGetUniformLocation(screen_shader.id, "u_screen_factor");
// 		u32 uloc_tex0_screen = glGetUniformLocation(screen_shader.id, "u_tex0"); 
// 		u32 uloc_tex1_screen = glGetUniformLocation(screen_shader.id, "u_tex1"); 
// 	Shader particle_shader = get_shader_program_VGF("res/particle.vert", "res/particle.geom", "res/particle2.frag"); AC(particle_shader)
// 		u32 uloc_tex_particle = glGetUniformLocation(screen_shader.id, "u_tex"); 
// 		u32 uloc_transform_particle = glGetUniformLocation(particle_shader.id, "u_transform");	
// 		u32 uloc_ssize_particle = glGetUniformLocation(particle_shader.id, "u_screen_size");
// 		u32 uloc_time_particle = glGetUniformLocation(particle_shader.id, "u_time");
// 		u32 uloc_psize_particle = glGetUniformLocation(particle_shader.id, "u_particle_size");
// 	float prev_time = glfwGetTime();	

// 	glEnable(GL_MULTISAMPLE);

// 	while(!glfwWindowShouldClose(main_window.window)) {
// 		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
// 		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
// 		glfwPollEvents();

// 		glm::mat4 projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
// 		glm::mat4 global_transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation);

// 		set_buffer(&frame_buffer);
// 			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer.depth_texture_id, 0);
// 			clear(vec3{1.f, 1.f ,1.f});
// 			/* slices */ {
// 				// glm::mat4 local_transform(1.f);
// 				// local_transform = glm::scale(local_transform, glm::vec3(1.7f));
// 				// glm::mat4 transform = global_transform * local_transform;

// 				// glUseProgram(blob_shader);
// 				// 	glUniformMatrix4fv(uloc_blob_transform, 1, GL_FALSE, glm::value_ptr(transform));

// 				// glEnable(GL_DEPTH_TEST); DEFER(glDisable(GL_DEPTH_TEST);)
// 				// 	draw(blob_mesh);
// 			}
// 		set_window_buffer(&main_window);

// 		clear(vec3{1.f, 1.f ,1.f});
// 		/* screen */ {
// 			glUseProgram(screen_shader.id);
// 				set_uniform_texture(uloc_tex0_screen, frame_buffer.color_texture_id, 0);
// 				set_uniform_texture(uloc_tex1_screen, frame_buffer.depth_texture_id, 1);
// 				glUniform2f(uloc_screen_factor_screen, (float)main_window.width / frame_buffer.width, (float)main_window.height / frame_buffer.height);

// 			glDisable(GL_DEPTH_TEST);
// 				draw_uv(quad_mesh);
// 		}

// 		/* particles */ {
// 			glm::mat4 local_transform(1.f);
// 			local_transform = glm::scale(local_transform, glm::vec3(1.7f));
// 			glm::mat4 transform = global_transform * local_transform;


// 			glUseProgram(particle_shader.id);
// 				glUniformMatrix4fv(uloc_transform_particle, 1, GL_FALSE, glm::value_ptr(transform));
// 				glActiveTexture(GL_TEXTURE0);
// 					glBindTexture(GL_TEXTURE_2D, frame_buffer.depth_texture_id);
// 				glUniform1i(uloc_tex_particle, 0);
// 				glUniform2f(uloc_ssize_particle, (float)main_window.width, (float)main_window.height);
// 				glUniform1f(uloc_time_particle, new_time);
// 				glUniform1f(uloc_psize_particle, 150);

// 			glEnable(GL_DEPTH_TEST); 
// 			glEnable(GL_BLEND);
// 			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// 				draw_particles(particle_cloud);
// 		}
// 		glfwSwapBuffers(main_window.window);		
// 	}
// 	return 0;
// }
// int main_5() {
// 	Window main_window(1200, 800);

// 	glfwSetKeyCallback(main_window.window, key_callback);

// 	u32 screen_tex;
// 	glCreateTextures(GL_TEXTURE_2D, 1, &screen_tex);
// 	glTextureParameteri(screen_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// 	glTextureParameteri(screen_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTextureParameteri(screen_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 	glTextureParameteri(screen_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 	glTextureStorage2D(screen_tex, 1, GL_RGBA32F, 1200, 800);
// 	glBindImageTexture(0, screen_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

// 	float quad_points[] = {
// 		-1.f, -1.f, 0.f, 0.f, 0.f,
// 		-1.f,  1.f, 0.f, 0.f, 1.f,
// 		 1.f,  1.f, 0.f, 1.f, 1.f,
// 		 1.f, -1.f, 0.f, 1.f, 0.f,
// 	};
// 	u32 quad_ids[] = {
// 		0, 2, 1,
// 		0, 3, 2
// 	};

// 	auto quad_mesh = make_mesh(link_mesh_uv, quad_points, 4, quad_ids, 6);


// 	//shaders
// 	Shader screen_shader = get_shader_program_VF("res/screen2.vert", "res/screen2.frag"); AC(screen_shader)
// 		u32 uloc_tex_screen = glGetUniformLocation(screen_shader.id, "u_screen"); 
// 	Shader compute_shader = get_shader_program_C("res/compute.comp"); AC(compute_shader);
// 		u32 uloc_time_comp = glGetUniformLocation(compute_shader.id, "u_time");

// 	float prev_time = glfwGetTime();	

// 	// glEnable(GL_MULTISAMPLE);


// 	while(!glfwWindowShouldClose(main_window.window)) {
// 		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
// 		glfwPollEvents();

// 		clear();

// 		glUseProgram(compute_shader.id);
// 			glUniform1f(uloc_time_comp, new_time);
// 			glDispatchCompute(1200 / 8, 800 / 4, 1);
// 			glMemoryBarrier(GL_ALL_BARRIER_BITS);

// 		glUseProgram(screen_shader.id);
// 			set_uniform_texture(uloc_tex_screen, screen_tex);

// 		draw_uv(quad_mesh);

// 		glfwSwapBuffers(main_window.window);
// 	}
// 	return 0;
// }

// int main_6() {
// 	Window main_window(1200, 800);

// 	glfwSetKeyCallback(main_window.window, key_callback);

// 	u32 screen_tex;
// 	glCreateTextures(GL_TEXTURE_2D, 1, &screen_tex);
// 	glTextureParameteri(screen_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// 	glTextureParameteri(screen_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// 	glTextureParameteri(screen_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 	glTextureParameteri(screen_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


// //  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
// 	glTextureStorage2D(screen_tex, 1, GL_RGBA32F, 1200, 800);


// 	glBindImageTexture(0, screen_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

// 	float quad_points[] = {
// 		-1.f, -1.f, 0.f, 0.f, 0.f,
// 		-1.f,  1.f, 0.f, 0.f, 1.f,
// 		 1.f,  1.f, 0.f, 1.f, 1.f,
// 		 1.f, -1.f, 0.f, 1.f, 0.f,
// 	};
// 	u32 quad_ids[] = {
// 		0, 2, 1,
// 		0, 3, 2
// 	};

// 	auto quad_mesh = make_mesh(link_mesh_uv, quad_points, 4, quad_ids, 6);


// 	//shaders
// 	Shader screen_shader = get_shader_program_VF("res/screen2.vert", "res/pendulum/display.frag"); AC(screen_shader)
// 		u32 uloc_tex_screen = glGetUniformLocation(screen_shader.id, "u_screen"); 
// 	Shader init_shader = get_shader_program_C("res/pendulum/init.comp"); AC(init_shader);
// 	Shader iterate_shader = get_shader_program_C("res/pendulum/iterate.comp"); AC(iterate_shader);
// 		u32 uloc_time_iterate = glGetUniformLocation(iterate_shader.id, "u_time");
// 		u32 uloc_dtime_iterate = glGetUniformLocation(iterate_shader.id, "u_dtime");

// 	glUseProgram(init_shader.id);
// 		// glUniform1f(uloc_time_comp, new_time);
// 		glDispatchCompute(1200 / 8, 800 / 4, 1);
// 		glMemoryBarrier(GL_ALL_BARRIER_BITS);

// 	while(!pressed_keys[GLFW_KEY_SPACE]) {
// 		glfwPollEvents();
// 	}


// 	glfwSetTime(0.f);
// 	float prev_time = glfwGetTime();	
// 	while(!glfwWindowShouldClose(main_window.window)) {
// 		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
// 		glfwPollEvents();

// 		clear();

// 		glUseProgram(iterate_shader.id);
// 			glUniform1f(uloc_time_iterate, new_time);
// 			glUniform1f(uloc_dtime_iterate, delta_time);
// 			glDispatchCompute(1200 / 8, 800 / 4, 1);
// 			glMemoryBarrier(GL_ALL_BARRIER_BITS);

// 		glUseProgram(screen_shader.id);
// 			set_uniform_texture(uloc_tex_screen, screen_tex);

// 		draw_uv(quad_mesh);

// 		glfwSwapBuffers(main_window.window);
// 	}
// 	return 0;
// }

// int main_7() {
// 	Window main_window(1200, 800);
// 	Camera main_camera;
// 	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
// 	Frame_Buffer frame_buffer(1200, 800);
// 	glfwSetKeyCallback(main_window.window, key_callback);

// 	float quad_points[] = {
// 		-1.f, -1.f, 0.f, 0.f, 0.f,
// 		-1.f,  1.f, 0.f, 0.f, 1.f,
// 		 1.f,  1.f, 0.f, 1.f, 1.f,
// 		 1.f, -1.f, 0.f, 1.f, 0.f,
// 	};
// 	u32 quad_ids[] = {
// 		0, 2, 1,
// 		0, 3, 2
// 	};

// 	auto quad_mesh = make_mesh(link_mesh_uv, quad_points, 4, quad_ids, 6);

// 	Model model("./res/6th_platonic_solid.obj"); AC(model);
// 	// Model model("./res/cat/scene.gltf"); AC(model);
// 	// Model model("./res/backpack/backpack.obj"); AC(model);

// 	//shaders
// 	Shader shader_model = get_shader_program_VF("res/default.vert", "res/default.frag"); AC(shader_model)
// 		u32 uloc_model_trworld = glGetUniformLocation(shader_model.id, "u_trworld");
// 		u32 uloc_model_trscreen = glGetUniformLocation(shader_model.id, "u_trscreen");
// 		// u32 uloc_teapot_view_pos = glGetUniformLocation(shader_default.id, "u_view_pos");
// 		// u32 uloc_teapot_tex_diff = glGetUniformLocation(shader_default.id, "u_tex_diff");
// 		// u32 uloc_teapot_tex_spec = glGetUniformLocation(shader_default.id, "u_tex_spec");
// 		// u32 uloc_teapot_tex_norm = glGetUniformLocation(shader_default.id, "u_tex_norm");
// 		// u32 uloc_teapot_time = glGetUniformLocation(shader_default.id, "u_time");
// 	Shader screen_shader = get_shader_program_VF("res/screen2.vert", "res/stereo.frag"); AC(screen_shader)
// 		u32 uloc_time_screen = glGetUniformLocation(screen_shader.id, "u_time"); 
// 		u32 uloc_tex_screen = glGetUniformLocation(screen_shader.id, "u_tex"); 
// 		u32 uloc_depth_screen = glGetUniformLocation(screen_shader.id, "u_depth"); 

// 	//Texture
// 	Texture pattern = load_texture("res/pattern.png");

// 	float prev_time = glfwGetTime();	
// 	while(!glfwWindowShouldClose(main_window.window)) {
// 		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
// 		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
// 		glfwPollEvents();
// 		glm::mat4 screen_transform = get_transform(&main_camera, &main_window);

// 		set_buffer(&frame_buffer);
// 		{
// 			glEnable(GL_DEPTH_TEST);
// 			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer.depth_texture_id, 0);
// 			// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer.color_texture_id, 0);
// 			clear(vec3{1.f, 1.f, 1.f});

// 			glm::mat4 world_transform(1.f);
// 			// world_transform = glm::scale(world_transform, glm::vec3(.2f, .2f, .2f));

// 			glUseProgram(shader_model.id);
// 				glUniformMatrix4fv(uloc_model_trworld, 1, GL_FALSE, (GLfloat*)&world_transform);
// 				glUniformMatrix4fv(uloc_model_trscreen, 1, GL_FALSE, (GLfloat*)&screen_transform);

// 			draw(model, 0, 0, 0);

// 		}

// 		set_window_buffer(&main_window);

// 		clear();

// 		glUseProgram(screen_shader.id);
// 			glUniform1f(uloc_time_screen, new_time);
// 			set_uniform(uloc_tex_screen, pattern, 0);
// 			set_uniform_texture(uloc_depth_screen, frame_buffer.depth_texture_id, 1);

// 		draw_uv(quad_mesh);

// 		glfwSwapBuffers(main_window.window);
// 	}
// 	return 0;
// }

int main(int argc, char * argv []) {
	// main_1();
	demo_0();
    return 0;
}




