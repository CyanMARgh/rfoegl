#include "demo_9.h"

#include "camera.h"
#include "primitives.h"
#include "shader.h"
#include "defer.h"
#include "image.h"

void demo_9() {
	const int width_screen = 1200, height_screen = 800, width_buf = 1200, height_buf = 800;
	Window main_window(width_screen, height_screen);

	set_default_key_callback(&main_window);

	Image buf_vawe_0(width_buf, height_buf, GL_RGBA32F);
	Image buf_vawe_1(width_buf, height_buf, GL_RGBA32F);
	Image buf_brightness(width_buf, height_buf, GL_RGBA32F);

	auto& quad_mesh = get_primitive(Primitive::QUAD);
	
	Shader screen_shader(Shader::VF, {"res/screen2.vert", "res/vawe/display.frag"});
	Shader init_shader(Shader::C, {"res/vawe/init.comp"});
	Shader iterate_shader(Shader::C, {"res/vawe/iterate.comp"});
	Shader accumulate_shader(Shader::C, {"res/vawe/accumulate.comp"});

	init_shader.use();
		init_shader.set("u_states", buf_vawe_0);
		glDispatchCompute(width_buf / 8, height_buf / 4, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	init_shader.use();
		init_shader.set("u_states", buf_vawe_1);
		glDispatchCompute(width_buf / 8, height_buf / 4, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	init_shader.use();
		init_shader.set("u_states", buf_brightness);
		glDispatchCompute(width_buf / 8, height_buf / 4, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

	bool direction = false;
	while(!pressed_keys[GLFW_KEY_SPACE]) {
		glfwPollEvents();
	}

	glfwSetTime(0.f);
	float prev_time = glfwGetTime(), new_time, delta_time;	

	auto compute = [&] (float t0, float dt) {
		iterate_shader.use();
			iterate_shader.set("u_states_in", direction ? buf_vawe_0 : buf_vawe_1, 0);
			iterate_shader.set("u_states_out", direction ? buf_vawe_1 : buf_vawe_0, 1);
			iterate_shader.set("u_time", t0);
			iterate_shader.set("u_dtime", dt);
			glDispatchCompute(width_buf / 8, height_buf / 4, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

		accumulate_shader.use();
			iterate_shader.set("u_vawe", direction ? buf_vawe_1 : buf_vawe_0, 0);
			iterate_shader.set("u_buf", buf_brightness, 1);
			glDispatchCompute(width_buf / 8, height_buf / 4, 1);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

		direction = !direction;
	};

	while(!glfwWindowShouldClose(main_window.window)) {
		glfwPollEvents();

		new_time = glfwGetTime(), delta_time = new_time - prev_time, prev_time = new_time;

		// for(int i = 0, n = 1; i < n; i++) {
		// 	compute(prev_time + i * delta_time / n, delta_time / n);
		// }
		compute(0, .01);
		
		clear();
		screen_shader.use();
			bind_texture(buf_brightness.id, 0);
		draw_uv(quad_mesh);

		glfwSwapBuffers(main_window.window);
	}
}