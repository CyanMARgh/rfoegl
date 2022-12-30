#include "demo_1.h"

#include "utils.h"
#include <vector>
#include "mesh.h"
#include "camera.h"
#include "defer.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>


vec3 fix_normal(vec3 n, vec3 v) {
	return normalize(n - v * dot(v, n) / dot(v, v));
}
void fix_path_normals(std::vector<Strip_Node>& strip) {
	u32 N = strip.size();
	for(u32 i = 1; i < N - 1; i++) {
		vec3 v = normalize(strip[i + 1].pos - strip[i].pos) + normalize(strip[i].pos - strip[i - 1].pos);
		strip[i].norm = fix_normal(strip[i].norm, v);
	}
	strip[0].norm = fix_normal(strip[0].norm, strip[1].pos - strip[0].pos);
	strip[N - 1].norm = fix_normal(strip[N - 1].norm, strip[N - 1].pos - strip[N - 2].pos);
}
void fix_cycle_normals(std::vector<Strip_Node>& strip) {
	u32 N = strip.size();
	for(u32 i = 0; i < N; i++) {
		vec3 v = normalize(strip[(i + 1) % N].pos - strip[i].pos) + normalize(strip[i].pos - strip[(i + N - 1) % N].pos);
		strip[i].norm = fix_normal(strip[i].norm, v);
	}
}
std::vector<Strip_Node> make_smoother_path(const std::vector<Strip_Node>& origin, float theta = 1.f / 3.f) {
	u32 N = origin.size();
	std::vector<Strip_Node> result(N * 2);

	result[0] = origin[0], result[N * 2 - 1] = origin[N - 1];
	for(u32 i = 0; i < N - 1; i++) {
		vec3 a = origin[i].pos, b = origin[i + 1].pos;
		vec3 an = origin[i].norm, bn = origin[i + 1].norm;
		result[i * 2 + 1].pos = lerp(a, b, theta);
		result[i * 2 + 1].norm = lerp(an, bn, theta);
		result[i * 2 + 2].pos = lerp(b, a, theta);
		result[i * 2 + 2].norm = lerp(bn, an, theta);
	}
	fix_path_normals(result);
	return result;
}
std::vector<Strip_Node> make_smoother_cycle(const std::vector<Strip_Node>& origin, float theta = 1.f / 3.f) {
	u32 N = origin.size();
	std::vector<Strip_Node> result(N * 2);

	for(u32 i = 0; i < N; i++) {
		vec3 a = origin[i].pos, b = origin[(i + 1) % N].pos;
		vec3 an = origin[i].norm, bn = origin[(i + 1) % N].norm;
		result[i * 2].pos = lerp(a, b, theta);
		result[i * 2].norm = lerp(an, bn, theta);
		result[i * 2 + 1].pos = lerp(b, a, theta);
		result[i * 2 + 1].norm = lerp(bn, an, theta);
	}
	fix_cycle_normals(result);
	return result;
}
std::vector<Strip_Node> generate_random_smooth_path(u32 count, u32* result_size_ptr) {
	u32 iterations = 5, result_size = count << iterations;
	*result_size_ptr = result_size;
	std::vector<Strip_Node> origin(count);

	for(u32 i = 0; i < count; i++) { origin[i] = {{randf(), randf(), (float)i / count}, rand_vec3_unit()}; }
	fix_path_normals(origin);
	for(u32 i = 0; i < iterations; i++) origin = make_smoother_path(origin);

	std::vector<Strip_Node> result(result_size + 2);
	result[0] = result[result_size + 1] = Strip_Node{.t = -1.f};
	for(u32 i = 0; i < result_size; i++) { result[i + 1] = origin[i]; }	
	
	return result;
}
std::vector<Strip_Node> generate_random_smooth_cycle(u32 count, u32* result_size_ptr) {
	u32 iterations = 5, result_size = count << iterations;
	*result_size_ptr = result_size + 1;

	std::vector<Strip_Node> origin(count);
	for(u32 i = 0; i < count; i++) { origin[i] = {rand_vec3() * 3.f, rand_vec3_unit()}; }

	fix_cycle_normals(origin);
	for(u32 i = 0; i < iterations; i++) origin = make_smoother_cycle(origin, .25f);

	std::vector<Strip_Node> result(result_size + 3);
	for(u32 i = 0; i < result_size; i++) { result[i] = origin[i]; }	
	result[result_size] = origin[0]; result[0].t = 1.f;
	result[result_size + 1] = origin[1]; 
	result[result_size + 2] = origin[2]; result[result_size + 2].t = 1.f;
	
	return result;
}

void demo_1() {
	Window main_window(1200, 800);
	set_default_key_callback(&main_window);

	Camera main_camera;
	main_camera.translation = glm::translate(main_camera.translation, glm::vec3(0.f, 0.f, 3.f));

	vec3 zone_size = {1.f, 1.f, 1.f};
	u32 POINTS_COUNT;
	std::vector<Strip_Node> points = generate_random_smooth_cycle(30, &POINTS_COUNT);

	float total_length = 0.f;
	for(u32 i = 1, n = points.size(); i < POINTS_COUNT + 1; i++) {
		points[i].t = total_length;
		if(i != POINTS_COUNT) total_length += glm::length((points[i + 1].pos - points[i].pos) / zone_size);
	}	

	auto line_strip = make_mesh<Strip_Node>(make_mesh_raw(points, {}));

	Shader shader_ls = get_shader_program_VGF("res/line_strip.vert", "res/line_strip.geom", "res/line_strip.frag"); AC(shader_ls)
		u32 uloc_ls_zs = glGetUniformLocation(shader_ls.id, "u_zone_size");
		u32 uloc_ls_pc = glGetUniformLocation(shader_ls.id, "u_point_count");
		u32 uloc_ls_length = glGetUniformLocation(shader_ls.id, "u_length");
		u32 uloc_ls_tex = glGetUniformLocation(shader_ls.id, "u_texture");
		u32 uloc_ls_ts = glGetUniformLocation(shader_ls.id, "u_tex_size");
		u32 uloc_ls_rad = glGetUniformLocation(shader_ls.id, "u_r");
		u32 uloc_ls_transform = glGetUniformLocation(shader_ls.id, "u_transform");
		u32 uloc_ls_time = glGetUniformLocation(shader_ls.id, "u_time");

	Texture texture = load_texture("res/chio_rio.png"); AC(texture)

	float prev_time = glfwGetTime();	
	while(!glfwWindowShouldClose(main_window.window)) {
		float new_time = glfwGetTime(), delta_time = new_time - prev_time;
		move_camera(&main_camera, pressed_keys, delta_time); prev_time = new_time;
		glfwPollEvents();
		clear();

		{
			glUseProgram(shader_ls.id);
				glUniform3f(uloc_ls_zs, zone_size.x, zone_size.y, zone_size.z);
				glUniform1i(uloc_ls_pc, POINTS_COUNT + 1);
				glUniform1f(uloc_ls_length, total_length);
				set_uniform(uloc_ls_tex, texture);
				glUniform2f(uloc_ls_ts, (float)texture.info.width, (float)texture.info.height);
				glUniform1f(uloc_ls_rad, .15f);
				glUniformMatrix4fv(uloc_ls_transform, 1, GL_FALSE, glm::value_ptr(get_transform(&main_camera, &main_window)));
				glUniform1f(uloc_ls_time, new_time);

			glEnable(GL_BLEND);
			glBlendEquation(GL_MAX);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				draw_line_strip(line_strip);
		}

		glfwSwapBuffers(main_window.window);		
	}
}
