#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include "utils.h"
#include "texture.h"
#include <vector>

struct Point_UV { vec3 pos = {0.f, 0.f, 0.f}; vec2 uv = {0.f, 0.f}; };
struct Point_N_UV { 
	vec3 pos = {0.f, 0.f, 0.f}; 
	vec2 uv = {0.f, 0.f};
	vec3 n1 = {0.f, 0.f, 0.f};
	vec3 n2 = {0.f, 0.f, 0.f};
	vec3 n3 = {0.f, 0.f, 0.f};
};
struct Particle { vec3 pos; };
struct Strip_Node {
	vec3 pos = {0.f, 0.f, 0.f};
	vec3 norm = {0.f, 0.f, 1.f};
	float t = -1.f;
};

struct Mesh_Raw {
	const float *points;
	const u32 *indices;
	u32 points_count, indices_count;
};
template<typename T>
Mesh_Raw make_mesh_raw(const std::vector<T>& points, const std::vector<u32>& indices) {
	return Mesh_Raw{(const float*)&(points[0]), &(indices[0]), (u32)points.size(), (u32)indices.size()};
}

struct Mesh_Any {
	std::function<void(Mesh_Raw, decltype(GL_STATIC_DRAW))> attr_linker; // TODO remove
	std::function<void(const Mesh_Any&)> painter;
	std::vector<Texture> textures;
	u32 vbo, vao, ebo, points_count, indices_count;
	bool is_owner, has_ebo;

	// TODO                                                                      u32
	void prepare(Mesh_Raw mesh_raw, const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode);

	Mesh_Any(
		std::function<void(Mesh_Raw, decltype(GL_STATIC_DRAW))> _attr_linker,
		std::function<void(const Mesh_Any&)> _painter,
		Mesh_Raw mesh_raw,
		const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode
	);

	Mesh_Any(const Mesh_Any&) = delete;
	Mesh_Any& operator=(const Mesh_Any&) = delete;

	void move(Mesh_Any& other);
	Mesh_Any(Mesh_Any&& other);
	Mesh_Any& operator=(Mesh_Any&& other);

	void clear();
	~Mesh_Any();

	void draw();
};

void link_mesh_default(Mesh_Raw mesh_raw, decltype(GL_STATIC_DRAW) draw_mode);
void link_particles(Mesh_Raw mesh_raw, decltype(GL_STATIC_DRAW) draw_mode);
void link_mesh_uv(Mesh_Raw mesh_raw, decltype(GL_STATIC_DRAW) draw_mode);
void link_line_strip(Mesh_Raw mesh_raw, decltype(GL_STATIC_DRAW) draw_mode);

void draw_default(const Mesh_Any& mesh);
void draw_uv(const Mesh_Any& mesh);
void draw_particles(const Mesh_Any& cloud);
void draw_line_strip(const Mesh_Any& strip);

template<typename T>
struct Mesh_Gen {
	static Mesh_Any make_mesh(Mesh_Raw mesh_raw, const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode) {
		printf("unknown vertex type!\n");
		exit(-1);
	}
};
template<>
struct Mesh_Gen<Point_N_UV> {
	static Mesh_Any make_mesh(Mesh_Raw mesh_raw, const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode) {
		return Mesh_Any(link_mesh_default, draw_default, mesh_raw, textures, draw_mode);
	}
};
template<>
struct Mesh_Gen<Point_UV> {
	static Mesh_Any make_mesh(Mesh_Raw mesh_raw, const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode) {
		return Mesh_Any(link_mesh_uv, draw_uv, mesh_raw, textures, draw_mode);
	}
};
template<>
struct Mesh_Gen<Particle> {
	static Mesh_Any make_mesh(Mesh_Raw mesh_raw, const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode) {
		return Mesh_Any(link_particles, draw_particles, mesh_raw, textures, draw_mode);
	}
};
template<>
struct Mesh_Gen<Strip_Node> {
	static Mesh_Any make_mesh(Mesh_Raw mesh_raw, const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode) {
		return Mesh_Any(link_line_strip, draw_line_strip, mesh_raw, textures, draw_mode);
	}
};

template<typename T>
Mesh_Any make_mesh(Mesh_Raw mesh_raw, const std::vector<Texture>& textures = {}, decltype(GL_STATIC_DRAW) draw_mode = GL_STATIC_DRAW) {
	return Mesh_Gen<T>::make_mesh(mesh_raw, textures, draw_mode);
}