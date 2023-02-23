#include "demo_2.h"


#include "utils.h"
#include <vector>
#include "camera.h"
#include "defer.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "framebuffer.h"
#include "primitives.h"


void demo_2() {
	Window main_window(1200, 800);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

	set_default_key_callback(&main_window);
	
	auto& quad_mesh = get_primitive(Primitive::QUAD);

	const u32 PARTICLES_COUNT = 200;
	std::vector<Particle> particles(PARTICLES_COUNT);
	for(u32 i = 0; i < PARTICLES_COUNT; i++) {
		vec2 p2 = rand_vec2_unit();
		particles[i].pos = vec3{p2.x, p2.y, 0.f} + (rand_vec3() * .3f - .15f);
	}

	Mesh_Any particle_cloud = make_mesh<Particle>(make_mesh_raw(particles, {}));

	Shader screen_shader(Shader::VF, {"res/only_uv.vert", "res/pass_color.frag"});
		u32 uloc_screen_factor_screen = glGetUniformLocation(screen_shader.id, "u_screen_factor");
		u32 uloc_tex0_screen = glGetUniformLocation(screen_shader.id, "u_tex0"); 
		u32 uloc_tex1_screen = glGetUniformLocation(screen_shader.id, "u_tex1"); 
	Shader particle_shader(Shader::VGF, {"res/particle.vert", "res/particle.geom", "res/particle2.frag", "SCREEN_SCALE"});
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

			particle_shader.use();
				particle_shader.set("u_transform", transform);
				particle_shader.set("u_screen_size", vec2{main_window.width, main_window.height});
				particle_shader.set("u_time", new_time);
				particle_shader.set("u_particle_size", 150.f);
			glEnable(GL_DEPTH_TEST); 
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				particle_cloud.draw();
		}
		glfwSwapBuffers(main_window.window);		
	}
}
