#include "demo_4.h"

#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "primitives.h"
#include "shader.h"
#include "defer.h"


void demo_4() {
	Window main_window(1200, 800);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

	set_default_key_callback(&main_window);

	u32 ac_buf;
	glGenBuffers(1, &ac_buf); DEFER(glDeleteBuffers(1, &ac_buf));
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ac_buf);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(u32), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);


	auto& cube_mesh = get_primitive(Primitive::CUBE);
	auto& quad_mesh = get_primitive(Primitive::QUAD);

	Shader cube_shader(Shader::VF, {"res/lamp.vert", "res/lamp.frag"});
	Shader screen_shader(Shader::VF, {"res/only_uv.vert", "res/pass_color.frag"});

	float prev_time = glfwGetTime();	

	glEnable(GL_MULTISAMPLE);

	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();

		glm::mat4 projection = glm::perspective(45.f, (float)main_window.width/(float)main_window.height, .1f, 100.f);
		glm::mat4 global_transform = projection * glm::inverse(main_camera.rotation) * glm::inverse(main_camera.translation);

		clear();
		/* cube */ {
			glm::mat4 local_transform(1.f);
			local_transform = glm::scale(local_transform, glm::vec3(1.7f));
			glm::mat4 transform = global_transform * local_transform;

			cube_shader.use();
				cube_shader.set("u_transform", transform);
				cube_shader.set("u_color", vec3{0.f, 1.f, 1.f});

			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, ac_buf);

			glEnable(GL_DEPTH_TEST); DEFER(glDisable(GL_DEPTH_TEST);)
				cube_mesh.draw();
		}
		glfwSwapBuffers(main_window.window);
	}
}