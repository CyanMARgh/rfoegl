#include "demo_3.h"

#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "defer.h"
#include "model.h"

// #define NORMALS
void demo_3() {
	Window main_window(1200, 800);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

	set_default_key_callback(&main_window);

	Model teapot("./res/backpack/backpack.obj"); AC(teapot);

	Shader shader_default(Shader::VF, {"res/default.vert", "res/default.frag"});
		u32 uloc_teapot_tex_diff = glGetUniformLocation(shader_default.id, "u_tex_diff"); //TODO ???
		u32 uloc_teapot_tex_spec = glGetUniformLocation(shader_default.id, "u_tex_spec");
		u32 uloc_teapot_tex_norm = glGetUniformLocation(shader_default.id, "u_tex_norm");
	Shader shader_normal(Shader::VGF, {"res/default.vert", "res/normals.geom", "res/lamp.frag"});

	float prev_time = glfwGetTime();	
	
	// glEnable(GL_CULL_FACE);  
	glEnable(GL_DEPTH_TEST);  
	glEnable(GL_MULTISAMPLE);
	
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glm::mat4 screen_transform = get_transform(&main_camera, &main_window);

		glfwPollEvents();
		clear(vec3(1.f, 0.f, 1.f));
		{
			glm::mat4 world_transform(1.f);
			// world_transform = glm::scale(world_transform, vec3(.1f, .1f, .1f));
			// world_transform = glm::translate(world_transform, vec3(0.f, -8.f, -4.f));
			// world_transform = glm::rotate(world_transform, new_time * 2.f, vec3(0.f, 1.f, 0.f));

			shader_default.use();
				shader_default.set("u_trworld", world_transform);
				shader_default.set("u_trscreen", screen_transform);
				shader_default.set("u_view_pos", get_position(main_camera));
				shader_default.set("u_time", new_time);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
				draw(teapot, uloc_teapot_tex_diff, uloc_teapot_tex_spec, uloc_teapot_tex_norm);
#ifdef NORMALS
			glUseProgram(shader_normals.id);
				glUniformMatrix4fv(uloc_normals_trworld, 1, GL_FALSE, (GLfloat*)&world_transform);
				glUniformMatrix4fv(uloc_normals_trscreen, 1, GL_FALSE, (GLfloat*)&screen_transform);

				draw(teapot, uloc_teapot_tex_diff, uloc_teapot_tex_spec, uloc_teapot_tex_norm);
#endif
		}

		glfwSwapBuffers(main_window.window);		
	}
}