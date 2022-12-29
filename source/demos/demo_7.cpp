#include "demo_7.h"

#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "defer.h"
#include "framebuffer.h"


void demo_7() {
	Window main_window(1200, 800);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));
	
	set_default_key_callback(&main_window);

	Frame_Buffer frame_buffer(1200, 800);

	float quad_points[] = {
		-1.f, -1.f, 0.f, 0.f, 0.f,
		-1.f,  1.f, 0.f, 0.f, 1.f,
		 1.f,  1.f, 0.f, 1.f, 1.f,
		 1.f, -1.f, 0.f, 1.f, 0.f,
	};
	u32 quad_ids[] = {0, 2, 1, 0, 3, 2};

	auto quad_mesh = make_mesh(link_mesh_uv, quad_points, 4, quad_ids, 6);

	Model model("./res/6th_platonic_solid.obj"); AC(model);
	// Model model("./res/cat/scene.gltf"); AC(model);
	// Model model("./res/backpack/backpack.obj"); AC(model);

	//shaders
	Shader shader_model = get_shader_program_VF("res/default.vert", "res/default.frag"); AC(shader_model)
		u32 uloc_model_trworld = glGetUniformLocation(shader_model.id, "u_trworld");
		u32 uloc_model_trscreen = glGetUniformLocation(shader_model.id, "u_trscreen");
		// u32 uloc_teapot_view_pos = glGetUniformLocation(shader_default.id, "u_view_pos");
		// u32 uloc_teapot_tex_diff = glGetUniformLocation(shader_default.id, "u_tex_diff");
		// u32 uloc_teapot_tex_spec = glGetUniformLocation(shader_default.id, "u_tex_spec");
		// u32 uloc_teapot_tex_norm = glGetUniformLocation(shader_default.id, "u_tex_norm");
		// u32 uloc_teapot_time = glGetUniformLocation(shader_default.id, "u_time");
	Shader screen_shader = get_shader_program_VF("res/screen2.vert", "res/stereo.frag"); AC(screen_shader)
		u32 uloc_time_screen = glGetUniformLocation(screen_shader.id, "u_time"); 
		u32 uloc_tex_screen = glGetUniformLocation(screen_shader.id, "u_tex"); 
		u32 uloc_depth_screen = glGetUniformLocation(screen_shader.id, "u_depth"); 

	//Texture
	Texture pattern = load_texture("res/pattern.png");

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

			glUseProgram(shader_model.id);
				glUniformMatrix4fv(uloc_model_trworld, 1, GL_FALSE, (GLfloat*)&world_transform);
				glUniformMatrix4fv(uloc_model_trscreen, 1, GL_FALSE, (GLfloat*)&screen_transform);

			draw(model, 0, 0, 0);
		}

		set_window_buffer(&main_window);

		clear();

		glUseProgram(screen_shader.id);
			glUniform1f(uloc_time_screen, new_time);
			set_uniform(uloc_tex_screen, pattern, 0);
			set_uniform_texture(uloc_depth_screen, frame_buffer.depth_texture_id, 1);

		draw_uv(quad_mesh);

		glfwSwapBuffers(main_window.window);
	}
}