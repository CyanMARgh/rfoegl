#include "demo_5.h"

#include "window.h"
#include "primitives.h"
#include "shader.h"
#include "defer.h"
#include "image.h"

void demo_5() {
	u32 width = 1200, height = 800;
	Window main_window(width, height);
	set_default_key_callback(&main_window);

	Image buf_tex(width, height);
	auto& quad_mesh = get_primitive(Primitive::QUAD);

	Shader screen_shader(Shader::VF, {"res/screen2.vert", "res/screen2.frag"});
	Shader compute_shader(Shader::C, {"res/tests/compute.comp"});

	float prev_time = glfwGetTime();	

	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		glfwPollEvents();
		clear();
		compute_shader.use();
			compute_shader.set("u_screen", buf_tex, 0);
			compute_shader.set("u_time", new_time);			
		glDispatchCompute(width / 8, height / 4, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		screen_shader.use();
			bind_texture(buf_tex.id, 0);
		quad_mesh.draw();

		glfwSwapBuffers(main_window.window);
	}
}