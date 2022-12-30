#include "demo_0.h"

#include "utils.h"
#include <vector>
#include "mesh.h"
#include "camera.h"
#include "defer.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "marching_squares.h"
#include "framebuffer.h"
#include "perlin.h"

void demo_0() {
	Window main_window(1200, 800);
	Frame_Buffer frame_buffer(2400, 1600);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

	set_default_key_callback(&main_window);

	Point_UV quad_points[] = {
		{{-1, -1,  0},  {0, 0}},
		{{ 1, -1,  0},  {1, 0}},
		{{ 1,  1,  0},  {1, 1}},
		{{-1,  1,  0},  {0, 1}},
	};
	u32 quad_ids[] = {0, 1, 2, 0, 2, 3};
	auto quad_mesh = make_mesh<Point_UV>(Mesh_Raw{(float*)quad_points, quad_ids, 4, 6});

	PerlinNoise generator;
	auto [blob_mesh, line_set] = make_layers_mesh([&generator] (float x, float y, float z) { 
		x = x * 2 - 1, y = y * 2 - 1, z = z * 2 - 1;
		float r = sqrtf(x * x + y * y) + fabsf(z);
		float h0 = .9 - r;
		float dh = generator.noise((x + 5) * 10, (y + 5) * 10, (z + 5) * 10) * .2;
		return h0 + dh;
	}, 60, 60, 12);

	const u32 PARTICLES_COUNT = 5000;
	std::vector<Particle> particles = spawn_particles(&line_set, PARTICLES_COUNT, 0.015);
	Mesh_Any particle_cloud = make_mesh<Particle>(make_mesh_raw(particles, {}));

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
					blob_mesh.draw();
			}
		set_window_buffer(&main_window);

		clear();
		/* screen */ {
			glUseProgram(screen_shader.id);
				set_uniform_texture(uloc_tex0_screen, frame_buffer.color_texture_id, 0);
				set_uniform_texture(uloc_tex1_screen, frame_buffer.depth_texture_id, 1);
				glUniform2f(uloc_screen_factor_screen, (float)main_window.width / frame_buffer.width, (float)main_window.height / frame_buffer.height);

			// draw_uv(quad_mesh);
			quad_mesh.draw();
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
}
