#pragma once

#include "utils.h"

struct Point_UV { vec3 pos; vec2 uv = {0.f, 0.f}; };
struct Mesh_UV /*Point_UV*/ {
	u32 VBO, VAO, EBO, indices_count, points_size;
	bool active = true;

	Mesh_UV(Point_UV* points, u32 points_count, u32* indices, u32 indices_count);

	Mesh_UV(Mesh_UV&&);
	Mesh_UV(const Mesh_UV&) = delete;

	Mesh_UV& operator= (Mesh_UV&&);
	Mesh_UV& operator=(const Mesh_UV&) = delete;

	~Mesh_UV();
};

struct Particle { vec3 pos; };
struct Particle_Cloud /*Particle*/ {
	//TODO same as Mesh_UV, but no EBO and different draw()
	u32 VBO, VAO, particles_count;
	Particle_Cloud(Particle* particles, u32 particles_count);
	~Particle_Cloud();
};