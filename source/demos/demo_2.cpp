#include "demo_2.h"


#include "utils.h"
#include <vector>
#include "mesh.h"
#include "camera.h"
#include "defer.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "framebuffer.h"


void demo_2() {
	Window main_window(1200, 800);

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

		clear(vec3{1.f, 1.f ,1.f});

		/* particles */ {
			glm::mat4 local_transform(1.f);
			local_transform = glm::scale(local_transform, glm::vec3(1.7f));
			glm::mat4 transform = global_transform * local_transform;

			glUseProgram(particle_shader.id);
				glUniformMatrix4fv(uloc_transform_particle, 1, GL_FALSE, glm::value_ptr(transform));
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
}
