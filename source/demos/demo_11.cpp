#include "demo_11.h"

#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "primitives.h"
#include "defer.h"
#include "image.h"

#include <cstdio>

void demo_11() {
	u32 width = 1200, height = 800;
	Window main_window(width, height);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	
	set_default_key_callback(&main_window);

	Image buf_heads(width, height, GL_R32UI);

	struct Color_Node {
		vec4 color;
		u32 prev;
		float depth;
	};

	// ssbo for lists
	const u32 max_pixels = width * height * 15;
	u32 ssbo_nodes;
	glGenBuffers(1, &ssbo_nodes);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_nodes);
		glBufferData(GL_SHADER_STORAGE_BUFFER, max_pixels * sizeof(Color_Node), NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_nodes);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// node counter
	u32 acbo_counter = 0;
	glGenBuffers(1, &acbo_counter); DEFER(glDeleteBuffers(1, &acbo_counter));
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, acbo_counter);
		glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(u32), NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, acbo_counter);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	auto clear_counter = [] (u32 id) {
		u32 zero = 0;
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, id);
		glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32), &zero);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	};

	auto& quad_mesh = get_primitive(Primitive::QUAD);
	Model model("./res/cat/scene.gltf"); AC(model);

	// Shader shader_to_nodes(Shader::VF, {"res/default.vert", "res/oit/to_nodes.frag"});
	// 	u32 i0 = shader_to_nodes.find("u_tex_diff"), i1 = shader_to_nodes.find("u_tex_spec"), i2 = shader_to_nodes.find("u_tex_norm");

	Shader shader_from_nodes(Shader::VF, {"res/screen2.vert", "res/oit/from_nodes.frag"});
	Shader shader_clear(Shader::C, {"res/oit/clear.comp"});

	Shader shader_particles(Shader::VGF, {"res/surface_particles/sp.vert", "res/surface_particles/sp.geom", "res/surface_particles/sp.frag"});
		u32 i0 = shader_particles.find("u_tex_diff");
		GLenum err = glGetError();
		printf("err = %x\n", err);
		u32 i1 = shader_particles.find("u_tex_spec"), i2 = shader_particles.find("u_tex_norm");
	printf("%d %d %d\n", i0, i1, i2);


	float prev_time = glfwGetTime();
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();
		glm::mat4 screen_transform = get_transform(&main_camera, &main_window);

		shader_clear.use();
			shader_clear.set("u_heads", buf_heads, 0);
			glDispatchCompute(width / 8, height / 4, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
		clear_counter(acbo_counter);

		// scene
		{
			// glEnable(GL_DEPTH_TEST); DEFER(glEnable(GL_DEPTH_TEST);)

			clear();
			glm::mat4 world_transform(1.f);
			world_transform = glm::scale(world_transform, glm::vec3(.2f, .2f, .2f));

			shader_particles.use();
				shader_particles.set("u_trworld", world_transform);
				shader_particles.set("u_trscreen", screen_transform);
				shader_particles.set("u_heads", buf_heads, 0);
				shader_particles.set("u_eye", get_position(main_camera));

			draw(model);
		}

		clear();
		shader_from_nodes.use();
			shader_from_nodes.set("u_heads", buf_heads, 0);
		draw_uv(quad_mesh);
		glfwSwapBuffers(main_window.window);
	}
}