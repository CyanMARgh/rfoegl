#include "demo_6.h"

#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "defer.h"


void demo_6() {
	Window main_window(1200, 800);

	set_default_key_callback(&main_window);

	u32 screen_tex;
	glCreateTextures(GL_TEXTURE_2D, 1, &screen_tex);
	glTextureParameteri(screen_tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screen_tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screen_tex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screen_tex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screen_tex, 1, GL_RGBA32F, 1200, 800);


	glBindImageTexture(0, screen_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	float quad_points[] = {
		-1.f, -1.f, 0.f, 0.f, 0.f,
		-1.f,  1.f, 0.f, 0.f, 1.f,
		 1.f,  1.f, 0.f, 1.f, 1.f,
		 1.f, -1.f, 0.f, 1.f, 0.f,
	};
	u32 quad_ids[] = {
		0, 2, 1,
		0, 3, 2
	};

	auto quad_mesh = make_mesh<Point_UV>(Mesh_Raw{(float*)quad_points, quad_ids, 4, 6});

	//shaders
	Shader screen_shader = get_shader_program_VF("res/screen2.vert", "res/pendulum/display.frag"); AC(screen_shader)
		u32 uloc_tex_screen = glGetUniformLocation(screen_shader.id, "u_screen"); 
	Shader init_shader = get_shader_program_C("res/pendulum/init.comp"); AC(init_shader);
	Shader iterate_shader = get_shader_program_C("res/pendulum/iterate.comp"); AC(iterate_shader);
		u32 uloc_time_iterate = glGetUniformLocation(iterate_shader.id, "u_time");
		u32 uloc_dtime_iterate = glGetUniformLocation(iterate_shader.id, "u_dtime");

	glUseProgram(init_shader.id);
		glDispatchCompute(1200 / 8, 800 / 4, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

	while(!pressed_keys[GLFW_KEY_SPACE]) {
		glfwPollEvents();
	}

	glfwSetTime(0.f);
	float prev_time = glfwGetTime();	
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		glfwPollEvents();

		glUseProgram(iterate_shader.id);
			glUniform1f(uloc_time_iterate, new_time);
			glUniform1f(uloc_dtime_iterate, delta_time);
			glDispatchCompute(1200 / 8, 800 / 4, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

		clear();
		glUseProgram(screen_shader.id);
			set_uniform_texture(uloc_tex_screen, screen_tex);

		draw_uv(quad_mesh);

		glfwSwapBuffers(main_window.window);
	}
}