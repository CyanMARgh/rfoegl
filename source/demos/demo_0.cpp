#include "demo_0.h"

#include "utils.h"
#include <vector>
#include "camera.h"
#include "defer.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "marching_squares.h"
#include "framebuffer.h"
#include "perlin.h"
#include "primitives.h"

void demo_0() {
	Window main_window(1200, 800);
	Frame_Buffer frame_buffer(main_window.width * 2, main_window.height * 2);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

	set_default_key_callback(&main_window);

	auto& quad_mesh = get_primitive(Primitive::QUAD);

	PerlinNoise generator;
	auto [blob_mesh, line_set] = make_layers_mesh([&generator] (float x, float y, float z) { 
		x = x * 2 - 1, y = y * 2 - 1, z = z * 2 - 1;
		float r = sqrtf(x * x + y * y) + fabsf(z);
		float h0 = .9 - r;
		float dh = generator.noise((x + 5) * 10, (y + 5) * 10, (z + 5) * 10) * .2;
		return h0 + dh;
	}, 60, 60, 12);

	const u32 PARTICLES_COUNT = 10000;
	std::vector<Particle> particles = spawn_particles(&line_set, PARTICLES_COUNT, 0.015);
	Mesh_Any particle_cloud = make_mesh<Particle>(make_mesh_raw(particles, {}));

	//shaders
	Shader blob_shader(Shader::VF, {"res/single_color.vert", "res/single_color.frag"});
	Shader screen_shader(Shader::VF, {"res/only_uv.vert", "res/pass_color.frag"});
	//TODO move to folder
	Shader particle_shader(Shader::VGF, {"res/particle.vert", "res/particle.geom", "res/particle.frag"});

	float prev_time = glfwGetTime();	

	glEnable(GL_MULTISAMPLE);

	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();

		glm::mat4 projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
		glm::mat4 global_transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation);

		set_buffer(&frame_buffer);
			// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer.depth_texture_id, 0);
			clear();
			/* slices */ {
				glm::mat4 local_transform(1.f);
				local_transform = glm::scale(local_transform, glm::vec3(1.7f));
				glm::mat4 transform = global_transform * local_transform;

				blob_shader.use();
					blob_shader.set("u_transform", transform);

				glEnable(GL_DEPTH_TEST); DEFER(glDisable(GL_DEPTH_TEST);)
					blob_mesh.draw();
			}
		set_window_buffer(&main_window);

		clear();
		/* screen */ {
			screen_shader.use();
				bind_texture(frame_buffer.color_texture_id, 0);
				// screen_shader.set_texture("u_color", frame_buffer.color_texture_id, 0);
			quad_mesh.draw();
		}
		/* particles */ {
			glm::mat4 local_transform(1.f);
			local_transform = glm::scale(local_transform, glm::vec3(1.7f));
			glm::mat4 transform = global_transform * local_transform;

			particle_shader.use();
				particle_shader.set("u_transform", transform);
				// particle_shader.set_texture("u_tex", frame_buffer.depth_texture_id);
				bind_texture(frame_buffer.depth_texture_id, 0);
				particle_shader.set("u_screen_size", vec2{(float)main_window.width, (float)main_window.height});
				particle_shader.set("u_time", new_time);
				particle_shader.set("u_particle_size", 20);

			glDepthMask(GL_FALSE); DEFER(glDepthMask(GL_TRUE);)
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				particle_cloud.draw();
		}

		glfwSwapBuffers(main_window.window);		
	}
}
