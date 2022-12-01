#pragma once

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

// struct Mesh_Any {
// 	u32 vbo, vao, ebo, indices_count, points_count;
// 	bool has_ebo;
// };
// void prepare_mesh(Mesh_Any& mesh, u32* indices = nullptr, u32 indices_count = 0, const std::vector<Texture>& textures = {}, decltype(GL_STATIC_DRAW) draw_mode = GL_STATIC_DRAW) {
// 	mesh.indices_count = indices_count;

// 	glGenVertexArrays(1, &VAO);
// 	glGenBuffers(1, &VBO);
// 	glBindVertexArray(VAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, VBO);

// 	if(!indices) {
// 		glGenBuffers(1, &EBO);
// 		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
// 		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(u32), indices, draw_mode);
// 	}
// 	mesh.has_ebo = !!indices;

// 	mesh.points_count = points_count;
// 	mesh.textures = textures;
// }
// void link_mesh_default(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode = GL_STATIC_DRAW) {
// 	glBufferData(GL_ARRAY_BUFFER, points_count * sizeof(Point_N_UV), points, GL_STATIC_DRAW);
// 	ADD_ATTRIB(0, Point_N_UV, pos);
// 	ADD_ATTRIB(1, Point_N_UV, uv);
// 	ADD_ATTRIB(2, Point_N_UV, n1);
// 	ADD_ATTRIB(3, Point_N_UV, n2);
// 	ADD_ATTRIB(4, Point_N_UV, n3);
// }

// void make_mesh(
// 	std::function<void(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode)> attr_linker, 
// 	void* points, u32 points_count,
// 	u32* indices = nullptr, u32 indices_count = 0, const std::vector<Texture>& textures = {}, decltype(GL_STATIC_DRAW) draw_mode = GL_STATIC_DRAW) {
	
// 	Mesh_Any mesh;
// 	prepare_mesh(mesh, indices, indices_count, textures, draw_mode);
// 	attr_linker(points, points_count, draw_mode);
// 	glBindVertexArray(0)
// }
// void clear(Mesh_Any mesh) {
// 	glDeleteVertexArrays(1, &VAO);
// 	glDeleteBuffers(1, &VBO);
// 	if(mesh.has_ebo) { glDeleteBuffers(1, &EBO); }
// }

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
	std::vector<Texture> textures;
	bool active = true;

	Mesh_N_UV(Point_N_UV* points, u32 points_count, u32* indices, u32 indices_count, std::vector<Texture> textures = {});

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
void draw(const Mesh_N_UV& mesh_n_uv, u32 uloc_diff, u32 uloc_spec, u32 uloc_norm);
void draw(const Particle_Cloud& particle_cloud);
void draw(const Line_Strip& line_strip);