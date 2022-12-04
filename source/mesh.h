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

struct Mesh_Any {
	u32 vbo, vao, ebo, indices_count, points_count;
	std::vector<Texture> textures;
	bool has_ebo;
};
void prepare_mesh(Mesh_Any& mesh, u32* indices, u32 indices_count, u32 points_count, const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode);

Mesh_Any make_mesh(
	std::function<void(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode)> attr_linker,
	void* points, u32 points_count,
	u32* indices = nullptr, u32 indices_count = 0,
	const std::vector<Texture>& textures = {}, decltype(GL_STATIC_DRAW) draw_mode = GL_STATIC_DRAW);

void clear(Mesh_Any& mesh);


void link_mesh_default(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode);
void link_particles(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode);
void link_mesh_uv(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode);
void link_line_strip(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode);

void draw_default(const Mesh_Any& mesh, u32 uloc_diff, u32 uloc_spec, u32 uloc_norm);
void draw_uv(const Mesh_Any& mesh);
void draw_particles(const Mesh_Any& cloud);
void draw_line_strip(const Mesh_Any& strip);






