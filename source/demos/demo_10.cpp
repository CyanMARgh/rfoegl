#include "demo_10.h"

#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "primitives.h"
#include "defer.h"
#include "framebuffer.h"
#include "image.h"

#include <cstdio>

const u32 max_pixels = 1200 * 800;
struct Pixel {
	vec4 color;
};

struct Shader_Data {
	// float colors[4] = {0.f, .2f, .5f, 1.f};
	vec4 colors[4] = {{1.f, 0.f, 0.f, 1.f}, {0.f, 1.f, 0.f, 1.f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 1.f, 0.f, 1.f}};
	// vec3 colors[4] = {{1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}, {1.f, 1.f, 0.f}};
} shader_data;

void demo_10() {
	u32 width = 1200, height = 800;
	Window main_window(width, height);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	
	set_default_key_callback(&main_window);

	Image buf_screen(width, height, GL_RGBA32F);

	// ubo
	u32 ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Shader_Data), &shader_data, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//ssbo for lists
	// u32 ssbo;
	// glGenBuffers(1, &ssbo);
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	// 	glBufferData(GL_SHADER_STORAGE_BUFFER, max_pixels * sizeof(Pixel), NULL, GL_DYNAMIC_DRAW);
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	auto& quad_mesh = get_primitive(Primitive::QUAD);
	
	// Model model("./res/6th_platonic_solid.obj"); AC(model);
	Model model("./res/cat/scene.gltf"); AC(model);
	// Model model("./res/backpack/backpack.obj"); AC(model);

	//shaders
	Shader shader_model(Shader::VF, {"res/default.vert", "res/oit/proj.frag"});
		u32 i0 = shader_model.find("u_tex_diff"), i1 = shader_model.find("u_tex_spec"), i2 = shader_model.find("u_tex_norm");
		u32 uloc_colors = glGetUniformBlockIndex(shader_model.id, "Shader_Data");
		glUniformBlockBinding(shader_model.id, uloc_colors, 0);

		// printf("%d %d %d\n", i0, i1, i2);	
	// Shader screen_shader(Shader::VF, {"res/screen2.vert", "res/stereo.frag"});
	Shader shader_screen_d4(Shader::VF, {"res/screen2.vert", "res/filters/screen.frag", "DIM 4"});
	Shader shader_clear(Shader::C, {"res/oit/clear.comp"});

	//Texture
	Texture pattern = _load_texture2("res/pattern.png");

	float prev_time = glfwGetTime();
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();
		glm::mat4 screen_transform = get_transform(&main_camera, &main_window);

		shader_clear.use();
			shader_clear.set("u_screen", buf_screen, 0);
			glDispatchCompute(width / 8, height / 4, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

		{
			glEnable(GL_DEPTH_TEST);
			// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer.depth_texture_id, 0);
			// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer.color_texture_id, 0);
			clear(vec3{1.f, 1.f, 1.f});

			glm::mat4 world_transform(1.f);
			world_transform = glm::scale(world_transform, glm::vec3(.2f, .2f, .2f));

			shader_model.use();
				shader_model.set("u_trworld", world_transform);
				shader_model.set("u_trscreen", screen_transform);
				shader_model.set("u_screen", buf_screen, 3);

				glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

			draw(model, i0, i1, i2);
		}

		// shader_screen_d4.use();
		// 	shader_screen_d4.set("u_screen", buf_screen);
		// draw_uv(quad_mesh);
		glfwSwapBuffers(main_window.window);
	}
}