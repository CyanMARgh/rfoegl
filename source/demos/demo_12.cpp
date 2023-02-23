#include "demo_12.h"

#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "primitives.h"
#include "defer.h"
#include "image.h"

#include <cstdio>

void demo_12() {
	auto print_err = [] () {
		u32 err = glGetError();
		printf("err = %x\n", err);
	};
	u32 width = 1200, height = 800;
	Window main_window(width, height);

	set_default_key_callback(&main_window);

	Image buf_brightness(width, height, GL_R32UI);

	auto& quad_mesh = get_primitive(Primitive::QUAD);
	// Model model("./res/cat/scene.gltf"); AC(model);

	Shader shader_genpixels(Shader::C, {"res/buddhabrot/gen.comp"});
	Shader shader_to_color(Shader::VF, {"res/screen2.vert", "res/buddhabrot/to_color.frag"});

	u32 time = 0;
	glfwSetTime(0.f);
	float prev_time = glfwGetTime();
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time; prev_time = new_time;

		glfwPollEvents();

		// add brightness
		if(time < 70) {
			time++;
			shader_genpixels.use();
				shader_genpixels.set("u_brbuf", buf_brightness, 0);
				shader_genpixels.set("u_time", time);
				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
		} else if(time == 70) {
			time++;
			printf("total time: %f\n", new_time);
		}

		// render result
		clear();
		shader_to_color.use();
			shader_to_color.set("u_brbuf", buf_brightness, 0);
			shader_to_color.set("u_time", time);
		draw_uv(quad_mesh);
		glfwSwapBuffers(main_window.window);
	}
}