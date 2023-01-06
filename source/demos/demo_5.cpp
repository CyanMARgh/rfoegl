#include "demo_5.h"

#include "window.h"
#include "primitives.h"
#include "shader.h"
#include "defer.h"

void demo_5() {
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

	Shader screen_shader(Shader::VF, {"res/screen2.vert", "res/screen2.frag"});
	Shader compute_shader(Shader::C, {"res/compute.comp"});

	float prev_time = glfwGetTime();	

	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		glfwPollEvents();

		clear();

		compute_shader.use();
			compute_shader.set("u_time", new_time);			
		glDispatchCompute(1200 / 8, 800 / 4, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		screen_shader.use();
			screen_shader.set_texture("u_screen", screen_tex);
		quad_mesh.draw();

		glfwSwapBuffers(main_window.window);
	}
}