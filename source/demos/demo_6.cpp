#include "demo_6.h"

#include "camera.h"
#include "primitives.h"
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

	auto& quad_mesh = get_primitive(Primitive::QUAD);
	
	Shader screen_shader(Shader::VF, {"res/screen2.vert", "res/pendulum/display.frag"});
	Shader init_shader(Shader::C, {"res/pendulum/init.comp"});
	Shader iterate_shader(Shader::C, {"res/pendulum/iterate.comp"});

	init_shader.use();
		glDispatchCompute(1200 / 8, 800 / 4, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

	while(!pressed_keys[GLFW_KEY_SPACE]) {
		glfwPollEvents();
	}

	glfwSetTime(0.f);
	float prev_time = glfwGetTime();	
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		prev_time = new_time;
		glfwPollEvents();

		iterate_shader.use();
			iterate_shader.set("u_time", new_time);
			iterate_shader.set("u_dtime", delta_time);
			glDispatchCompute(1200 / 8, 800 / 4, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

		clear();
		
		screen_shader.use();
			bind_texture(screen_tex, 0);
		draw_uv(quad_mesh);

		glfwSwapBuffers(main_window.window);
	}
}