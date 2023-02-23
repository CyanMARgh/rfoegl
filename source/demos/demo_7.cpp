#include "demo_7.h"

#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "primitives.h"
#include "defer.h"
#include "framebuffer.h"

#include <cstdio>

void demo_7() {
	Window main_window(1200, 800);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	
	set_default_key_callback(&main_window);

	Frame_Buffer frame_buffer(1200, 800);

	auto& quad_mesh = get_primitive(Primitive::QUAD);
	
	Model model("./res/6th_platonic_solid.obj"); AC(model);
	// Model model("./res/cat/scene.gltf"); AC(model);
	// Model model("./res/backpack/backpack.obj"); AC(model);

	//shaders
	Shader shader_model(Shader::VF, {"res/default.vert", "res/default.frag"});
	Shader screen_shader(Shader::VF, {"res/screen2.vert", "res/stereo.frag"});

	//Texture
	Texture pattern = _load_texture2("res/pattern.png");

	float prev_time = glfwGetTime();
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();
		glm::mat4 screen_transform = get_transform(&main_camera, &main_window);

		set_buffer(&frame_buffer); {
			glEnable(GL_DEPTH_TEST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer.depth_texture_id, 0);
			// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_buffer.color_texture_id, 0);
			clear(vec3{1.f, 1.f, 1.f});

			glm::mat4 world_transform(1.f);
			// world_transform = glm::scale(world_transform, glm::vec3(.2f, .2f, .2f));

			shader_model.use();
				shader_model.set("u_trworld", world_transform);
				shader_model.set("u_trscreen", screen_transform);

			draw(model/*, 0, 0, 0*/);
		}
		set_window_buffer(&main_window);

		clear();

		screen_shader.use();
			screen_shader.set("u_time", new_time);
			bind(pattern, 0);
			bind_texture(frame_buffer.depth_texture_id, 1);

		quad_mesh.draw();

		glfwSwapBuffers(main_window.window);
	}
}