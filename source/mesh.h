#pragma once

#include "utils.h"

struct Point_UV { vec3 pos = {0.f, 0.f, 0.f}; vec2 uv = {0.f, 0.f}; };
struct Point_N_UV {vec3 pos = {0.f, 0.f, 0.f}; vec3 norm = {0.f, 0.f, 0.f}; vec2 uv = {0.f, 0.f}; };

struct Mesh_UV /*Point_UV*/ {
	u32 VBO, VAO, EBO, indices_count, points_count;
	bool active = true;

	Mesh_UV(Point_UV* points, u32 points_count, u32* indices, u32 indices_count);

	Mesh_UV(Mesh_UV&&);
	Mesh_UV(const Mesh_UV&) = delete;

	Mesh_UV& operator= (Mesh_UV&&);
	Mesh_UV& operator=(const Mesh_UV&) = delete;

	~Mesh_UV();
};
struct Mesh_N_UV /*Point_N_UV*/ {
	u32 VBO, VAO, EBO, indices_count, points_count;
	bool active = true;

	Mesh_N_UV(Point_N_UV* points, u32 points_count, u32* indices, u32 indices_count);

	Mesh_N_UV(Mesh_N_UV&&);
	Mesh_N_UV(const Mesh_N_UV&) = delete;

	Mesh_N_UV& operator= (Mesh_N_UV&&);
	Mesh_N_UV& operator=(const Mesh_N_UV&) = delete;

	~Mesh_N_UV();
};

struct Particle { vec3 pos; };
struct Particle_Cloud /*Particle*/ {
	//TODO same as Mesh_UV, but no EBO and different draw()
	u32 VBO, VAO, particles_count;
	Particle_Cloud(Particle* particles, u32 particles_count);
	~Particle_Cloud();
};
struct Strip_Node {
	vec3 pos = {0.f, 0.f, 0.f};
	vec3 norm = {0.f, 0.f, 1.f};
	float t = -1.f;
};
struct Line_Strip {
	u32 VBO, VAO, points_count;
	bool active = true;

	Line_Strip(Strip_Node* points, u32 count);

	// Line_Strip(Line_Strip&&);
	Line_Strip(const Line_Strip&) = delete;

	// Line_Strip& operator= (Line_Strip&&);
	Line_Strip& operator=(const Line_Strip&) = delete;

	~Line_Strip();
};

void draw(const Mesh_UV& mesh_uv);
void draw(const Mesh_N_UV& mesh_n_uv);
void draw(const Particle_Cloud& particle_cloud);
void draw(const Line_Strip& line_strip);