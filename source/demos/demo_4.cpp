#include "demo_4.h"

#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "defer.h"


void demo_4() {
	Window main_window(1200, 800);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

	set_default_key_callback(&main_window);

	Point_UV cube_points[] = {
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f}},

		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

		{{ 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

		{{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}}
	};
	u32 cube_ids[36];
	for(u32 i = 0; i < 6; i++) {
		cube_ids[i*6  ] = i*4+0; 
		cube_ids[i*6+1] = i*4+1; 
		cube_ids[i*6+2] = i*4+2; 
		cube_ids[i*6+3] = i*4+0; 
		cube_ids[i*6+4] = i*4+2; 
		cube_ids[i*6+5] = i*4+3; 
	}	

	u32 ac_buf;
	glGenBuffers(1, &ac_buf); DEFER(glDeleteBuffers(1, &ac_buf));
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, ac_buf);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(u32), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);


	// auto cube_mesh = make_mesh(link_mesh_uv, cube_points, 24, cube_ids, 36);
	auto cube_mesh = make_mesh<Point_UV>(Mesh_Raw{(float*)cube_points, cube_ids, 24, 36});

	Point_UV quad_points[] = {
		{{-1, -1,  0},  {0, 0}},
		{{ 1, -1,  0},  {1, 0}},
		{{ 1,  1,  0},  {1, 1}},
		{{-1,  1,  0},  {0, 1}},
	};
	u32 quad_ids[] = {0, 1, 2, 0, 2, 3};
	auto quad_mesh = make_mesh<Point_UV>(Mesh_Raw{(float*)quad_points, quad_ids, 4, 6});

	//shaders
	Shader cube_shader = get_shader_program_VF("res/lamp.vert", "res/lamp.frag"); AC(cube_shader)
		u32 uloc_cube_transform = glGetUniformLocation(cube_shader.id, "u_transform");
		u32 uloc_cube_color = glGetUniformLocation(cube_shader.id, "u_color");
	Shader screen_shader = get_shader_program_VF("res/screen.vert", "res/screen.frag"); AC(screen_shader)
		u32 uloc_screen_factor_screen = glGetUniformLocation(screen_shader.id, "u_screen_factor");
		u32 uloc_tex0_screen = glGetUniformLocation(screen_shader.id, "u_tex0"); 
		u32 uloc_tex1_screen = glGetUniformLocation(screen_shader.id, "u_tex1");

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

			glUseProgram(cube_shader.id);
				glUniformMatrix4fv(uloc_cube_transform, 1, GL_FALSE, glm::value_ptr(transform));
				glUniform3f(uloc_cube_color, 0.f, 1.f, 1.f);

			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, ac_buf);

			glEnable(GL_DEPTH_TEST); DEFER(glDisable(GL_DEPTH_TEST);)
				draw_uv(cube_mesh);
		}
		glfwSwapBuffers(main_window.window);
	}
}