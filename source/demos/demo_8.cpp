#include "demo_8.h"

#include "camera.h"
#include "primitives.h"
#include "shader.h"
#include "defer.h"
#include "texture.h"
#include "image.h"

#include <fstream>
#include <cstdio>
#include <sys/stat.h>

struct Params_Refresher {
	std::string path;
	std::vector<float*> params = {};
	time_t mtime = 0;

	void add_param(float* p) {
		params.push_back(p);
	}
	void add_param(vec2* p) {
		params.push_back(&(p->x));
		params.push_back(&(p->y));
	}
	void add_param(vec3* p) {
		params.push_back(&(p->x));
		params.push_back(&(p->y));
		params.push_back(&(p->z));
	}
	void add_param(vec4* p) {
		params.push_back(&(p->x));
		params.push_back(&(p->y));
		params.push_back(&(p->z));
		params.push_back(&(p->w));
	}
	void refresh() {
		std::ifstream fin(path);
		if(fin.fail()) {
			printf("can't open file: %s\n", path.c_str());
		}
		for(u32 i = 0, n = params.size(); i < n; i++) {
			// fin >> *(params[i]);
			if(!(fin >> *(params[i]))) {
				printf("can't read argument #%d\n", i);
				return;
			}
		}
	}

	bool autorefresh() {
		FILE *fp;
		struct stat buff;
		if((fp = fopen(path.c_str(), "r")) == NULL) {
			printf("can't find file: %s\n", path.c_str());
			return false;
		}
		stat(path.c_str(), &buff);
		fclose(fp);
		if(mtime < buff.st_mtime) {
			refresh();
			mtime = buff.st_mtime;
			return true;
		}
		return false;
	}
};

void demo_8() {
	float sigma_flow = 1., sigma_high = 1., sigma_low = 1., tau = 1.;

	auto refresher = Params_Refresher{"res/demo_8/params"};
	refresher.add_param(&sigma_flow);
	refresher.add_param(&sigma_high);
	refresher.add_param(&sigma_low);
	refresher.add_param(&tau);

	const u32 width = 1000, height = 1000;
	Window main_window(width, height);
	set_default_key_callback(&main_window);

	Texture tex_input = load_texture("res/input_texture3.png");
	Image buf_grayscale(width, height, GL_R32F);
	Image buf_sobel(width, height, GL_RG32F);
	Image buf_sobel_gauss_h(width, height, GL_RG32F);
	Image buf_sobel_gauss_hv(width, height, GL_RG32F);
	Image buf_edge_gaussian(width, height, GL_R32F);

	
	auto& quad_mesh = get_primitive(Primitive::QUAD);
	
	Shader shader_grayscale(Shader::C, {"res/filters/grayscale.comp"});
	Shader shader_sobel(Shader::C, {"res/filters/sobel.comp"});
	// Shader shader_gauss_d1h(Shader::C, {"res/filters/gauss.comp", "DIM 1"});
	// Shader shader_gauss_d1v(Shader::C, {"res/filters/gauss.comp", "DIM 1", "VERTICAL"});
	Shader shader_gauss_d2h(Shader::C, {"res/filters/gauss.comp", "DIM 2"});
	Shader shader_gauss_d2v(Shader::C, {"res/filters/gauss.comp", "DIM 2", "VERTICAL"});
	// Shader shader_gauss_d4(Shader::C, {"res/filters/gauss.comp", "DIM 4"});
	Shader shader_edge_gauss_d1(Shader::C, {"res/filters/edge_gauss.comp", "DIM 1"});

	Shader shader_screen_d1(Shader::VF, {"res/screen2.vert", "res/filters/screen.frag", "DIM 1"});
	Shader shader_screen_d2(Shader::VF, {"res/screen2.vert", "res/filters/screen.frag", "DIM 2"});
	// Shader shader_screen_d4(Shader::VF, {"res/screen2.vert", "res/filters/screen.frag", "DIM 4"});

	float prev_time = glfwGetTime();
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time; prev_time = new_time;

		if(refresher.autorefresh()) {
			shader_grayscale.use();
				shader_grayscale.set_image("in_tex", tex_input.info.id, 0);
				shader_grayscale.set("out_tex", buf_grayscale, 1);

				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			shader_sobel.use();
				shader_sobel.set("in_tex", buf_grayscale, 0);
				shader_sobel.set("out_tex", buf_sobel, 1);

				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			shader_gauss_d2h.use();
				shader_gauss_d2h.set("u_sigma", sigma_flow);
				shader_gauss_d2h.set("in_tex", buf_sobel, 0);
				shader_gauss_d2h.set("out_tex", buf_sobel_gauss_h, 1);

				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			shader_gauss_d2v.use();
				shader_gauss_d2v.set("u_sigma", sigma_flow);
				shader_gauss_d2v.set("in_tex", buf_sobel_gauss_h, 0);
				shader_gauss_d2v.set("out_tex", buf_sobel_gauss_hv, 1);

				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			shader_edge_gauss_d1.use();
				shader_edge_gauss_d1.set("u_sigma_high", sigma_high);
				shader_edge_gauss_d1.set("u_sigma_low", sigma_low);
				shader_edge_gauss_d1.set("u_tau", tau);
				shader_edge_gauss_d1.set("dir_tex", buf_sobel_gauss_hv, 0);
				shader_edge_gauss_d1.set("in_tex", buf_grayscale, 1);
				shader_edge_gauss_d1.set("out_tex", buf_edge_gaussian, 2);

				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);


			// shader_gauss_d4.use();
			// 	shader_gauss_d1.set("u_sigma", sigma_0);
			// 	shader_gauss_d1.set_image("in_tex", tex_input.info.id, 0);
			// 	shader_gauss_d1.set("out_tex", buf_gauss, 1);

			// 	glDispatchCompute(width / 8, height / 4, 1);
			// 	glMemoryBarrier(GL_ALL_BARRIER_BITS);
		}

		glfwPollEvents();
		clear(vec3{1.f, 1.f, 0.f});
		shader_screen_d1.use();
			shader_screen_d1.set("in_tex", buf_edge_gaussian);
		// shader_screen_d2.use();
		// 	shader_screen_d2.set("in_tex", buf_sobel_gauss_hv);
		// shader_screen_d4.use();
		// 	shader_screen_d4.set("in_tex", buf_gauss);
		draw_uv(quad_mesh);

		glfwSwapBuffers(main_window.window);
	}
}

// void demo_8() {
// 	vec4 sigma = {1.5f, 5.f, 6.f, 7.f};
// 	float tau = 1;
// 	auto refresher = Params_Refresher{"res/demo_8/params"};
// 	refresher.add_param(&sigma);
// 	refresher.add_param(&tau);

// 	const u32 width = 1200, height = 800;
// 	Window main_window(width, height);
// 	set_default_key_callback(&main_window);
// 	Texture input_tex = load_texture("res/input_texture.png");

// 	Image buf_0(width, height);
// 	Image buf_1(width, height);
	
// 	auto& quad_mesh = get_primitive(Primitive::QUAD);
	
// 	Shader screen_shader(Shader::VF, {"res/screen2.vert", "res/filters/screen.frag"});
// 	Shader filter_hor(Shader::C, {"res/filters/gauss_first.comp"});
// 	Shader filter_vert(Shader::C, {"res/filters/gauss_second.comp"});

// 	float prev_time = glfwGetTime();
// 	while(!glfwWindowShouldClose(main_window.window)) {
// 		float new_time = glfwGetTime(), delta_time = new_time - prev_time; prev_time = new_time;

// 		if(refresher.autorefresh()) {
// 			filter_hor.use();
// 				filter_hor.set_image("in_tex", input_tex.info.id, 0);
// 				filter_hor.set("out_tex", buf_0, 1);
// 				filter_hor.set("u_sigma", sigma);

// 				glDispatchCompute(1200 / 8, 800 / 4, 1);
// 				glMemoryBarrier(GL_ALL_BARRIER_BITS);
// 			filter_vert.use();
// 				filter_vert.set("in_tex", buf_0, 0);
// 				filter_vert.set("out_tex", buf_1, 1);
// 				filter_vert.set("u_sigma", sigma);
			
// 				glDispatchCompute(1200 / 8, 800 / 4, 1);
// 				glMemoryBarrier(GL_ALL_BARRIER_BITS);
// 		}

// 		glfwPollEvents();
// 		clear(vec3{1.f, 1.f, 0.f});
// 		screen_shader.use();
// 			screen_shader.set_texture("u_screen", buf_1.id);
// 			screen_shader.set("u_time", new_time);
// 			screen_shader.set("u_tau", tau);
// 		draw_uv(quad_mesh);

// 		glfwSwapBuffers(main_window.window);
// 	}
// }