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
	float sigma_flow = 5.;//, sigma_high = 3., sigma_low = 5., tau = 0., eps = 0.;

	auto refresher = Params_Refresher{"res/demo_8/params"};
	refresher.add_param(&sigma_flow);
	// refresher.add_param(&sigma_high);
	// refresher.add_param(&sigma_low);
	// refresher.add_param(&tau);
	// refresher.add_param(&eps);

	const u32 width = 1200, height = 800;
	Window main_window(width, height);
	set_default_key_callback(&main_window);

	Texture tex_input = load_texture("res/input_texture.png");
	Image buf_grayscale(width, height, GL_R32F);
	// Image buf_grayscale_smooth_h(width, height, GL_R32F);
	// Image buf_grayscale_smooth_hv(width, height, GL_R32F);
	Image buf_sobel(width, height, GL_RGBA32F);
	Image buf_sobel_gauss_h(width, height, GL_RGBA32F);
	Image buf_sobel_gauss_hv(width, height, GL_RGBA32F);
	Image buf_dir_field(width, height, GL_RG32F);	
	// Image buf_final(width, height, GL_R32F);
	Image buf_final_rgba(width, height, GL_RGBA32F);
	// Image buf_edge_gaussian(width, height, GL_R32F);

	auto& quad_mesh = get_primitive(Primitive::QUAD);
	
	Shader shader_grayscale(Shader::C, {"res/filters/grayscale.comp"});
	Shader shader_sobel(Shader::C, {"res/filters/sobel.comp"});
	// Shader shader_gauss_d1h(Shader::C, {"res/filters/gauss.comp", "DIM 1"});
	// Shader shader_gauss_d1v(Shader::C, {"res/filters/gauss.comp", "DIM 1", "VERTICAL"});
	Shader shader_gauss_d4h(Shader::C, {"res/filters/gauss.comp", "DIM 4"});
	Shader shader_gauss_d4v(Shader::C, {"res/filters/gauss.comp", "DIM 4", "VERTICAL"});
	// Shader shader_edge_gauss_d1(Shader::C, {"res/filters/edge_gauss.comp", "DIM 1"});
	Shader shader_sobel_inv(Shader::C, {"res/filters/sobel_inv.comp"});
	// Shader shader_final(Shader::C, {"res/filters/final.comp"});
	Shader shader_kuwahara(Shader::C, {"res/filters/kuwahara.comp"});

	// Shader shader_screen_d1(Shader::VF, {"res/screen2.vert", "res/filters/screen.frag", "DIM 1"});
	Shader shader_screen_d2(Shader::VF, {"res/screen2.vert", "res/filters/screen.frag", "DIM 2"});
	Shader shader_screen_d4(Shader::VF, {"res/screen2.vert", "res/filters/screen.frag", "DIM 4"});

	float prev_time = glfwGetTime();
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time; prev_time = new_time;

		if(refresher.autorefresh()) {
			shader_kuwahara.use();
				shader_kuwahara.set_image("in_tex", tex_input.info.id, 0);
				shader_kuwahara.set("out_tex", buf_final_rgba, 1);
				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
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
			shader_gauss_d4h.use();
				shader_gauss_d4h.set("u_sigma", sigma_flow);
				shader_gauss_d4h.set("in_tex", buf_sobel, 0);
				shader_gauss_d4h.set("out_tex", buf_sobel_gauss_h, 1);
				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			shader_gauss_d4v.use();
				shader_gauss_d4v.set("u_sigma", sigma_flow);
				shader_gauss_d4v.set("in_tex", buf_sobel_gauss_h, 0);
				shader_gauss_d4v.set("out_tex", buf_sobel_gauss_hv, 1);
				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			// shader_edge_gauss_d1.use();
			// 	shader_edge_gauss_d1.set("u_sigma_high", sigma_high);
			// 	shader_edge_gauss_d1.set("u_sigma_low", sigma_low);
			// 	shader_edge_gauss_d1.set("u_tau", tau);
			// 	shader_edge_gauss_d1.set("u_eps", eps);
			// 	shader_edge_gauss_d1.set("dir_tex", buf_sobel_gauss_hv, 0);
			// 	shader_edge_gauss_d1.set("in_tex", buf_grayscale, 1);
			// 	shader_edge_gauss_d1.set("out_tex", buf_final, 2);
			// 	glDispatchCompute(width / 8, height / 4, 1);
			// 	glMemoryBarrier(GL_ALL_BARRIER_BITS);
			shader_sobel_inv.use();
				shader_sobel_inv.set("in_tex", buf_sobel_gauss_hv, 0);
				shader_sobel_inv.set("out_tex", buf_dir_field, 1);
				glDispatchCompute(width / 8, height / 4, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);			

			// shader_final.use();
			// 	shader_final.set("in_tex", buf_sobel_gauss_hv, 0);
			// 	shader_final.set("out_tex", buf_final, 1);
			// 	glDispatchCompute(width / 8, height / 4, 1);
			// 	glMemoryBarrier(GL_ALL_BARRIER_BITS);

			// shader_gauss_d2h.use();
			// 	shader_gauss_d2h.set("u_sigma", sigma_flow);
			// 	shader_gauss_d2h.set("in_tex", buf_sobel, 0);
			// 	shader_gauss_d2h.set("out_tex", buf_sobel_gauss_h, 1);

			// 	glDispatchCompute(width / 8, height / 4, 1);
			// 	glMemoryBarrier(GL_ALL_BARRIER_BITS);
			// shader_gauss_d2v.use();
			// 	shader_gauss_d2v.set("u_sigma", sigma_flow);
			// 	shader_gauss_d2v.set("in_tex", buf_sobel_gauss_h, 0);
			// 	shader_gauss_d2v.set("out_tex", buf_sobel_gauss_hv, 1);

			// 	glDispatchCompute(width / 8, height / 4, 1);
			// 	glMemoryBarrier(GL_ALL_BARRIER_BITS);
			// shader_edge_gauss_d1.use();
			// 	shader_edge_gauss_d1.set("u_sigma_high", sigma_high);
			// 	shader_edge_gauss_d1.set("u_sigma_low", sigma_low);
			// 	shader_edge_gauss_d1.set("u_tau", tau);
			// 	shader_edge_gauss_d1.set("dir_tex", buf_sobel_gauss_hv, 0);
			// 	shader_edge_gauss_d1.set("in_tex", buf_grayscale, 1);
			// 	shader_edge_gauss_d1.set("out_tex", buf_edge_gaussian, 2);

			// 	glDispatchCompute(width / 8, height / 4, 1);
			// 	glMemoryBarrier(GL_ALL_BARRIER_BITS);

			// shader_gauss_d4.use();
			// 	shader_gauss_d1.set("u_sigma", sigma_0);
			// 	shader_gauss_d1.set_image("in_tex", tex_input.info.id, 0);
			// 	shader_gauss_d1.set("out_tex", buf_gauss, 1);

			// 	glDispatchCompute(width / 8, height / 4, 1);
			// 	glMemoryBarrier(GL_ALL_BARRIER_BITS);
		}

		glfwPollEvents();
		clear(vec3{1.f, 1.f, 0.f});
		// shader_screen_d1.use();
		// 	shader_screen_d1.set("in_tex", buf_final);
		// shader_screen_d2.use();
		// 	shader_screen_d2.set("in_tex", buf_dir_field);
		shader_screen_d4.use();
			shader_screen_d4.set("in_tex", buf_final_rgba);
		draw_uv(quad_mesh);

		glfwSwapBuffers(main_window.window);
	}
}
