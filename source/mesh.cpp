#include <cstdio>

#include "mesh.h"
#include "defer.h"

#define ADD_ATTRIB(id, struct_name, field_name)\
glVertexAttribPointer(id, sizeof(std::declval<struct_name>().field_name) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(struct_name), (void*)offsetof(struct_name, field_name));\
glEnableVertexAttribArray(id);

void prepare_mesh(Mesh_Any& mesh, u32* indices, u32 indices_count, u32 points_count, const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode) {
	mesh.indices_count = indices_count;
	mesh.points_count = points_count;
	mesh.textures = textures;
	mesh.has_ebo = !!indices_count;

	glGenVertexArrays(1, &(mesh.vao));
	glGenBuffers(1, &(mesh.vbo));
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	if(mesh.has_ebo) {
		glGenBuffers(1, &(mesh.ebo));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(u32), indices, draw_mode);
	}
}
Mesh_Any make_mesh(
	std::function<void(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode)> attr_linker, 
	void* points, u32 points_count,
	u32* indices, u32 indices_count,
	const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode) {

	// printf("+ mesh\n");
	
	Mesh_Any mesh;
	prepare_mesh(mesh, indices, indices_count, points_count, textures, draw_mode);
	attr_linker(points, points_count, draw_mode);
	glBindVertexArray(0);

	return mesh;
}
void clear(Mesh_Any& mesh) {
	// printf("- mesh\n");
	glDeleteVertexArrays(1, &(mesh.vao));
	glDeleteBuffers(1, &(mesh.vbo));
	if(mesh.has_ebo) { glDeleteBuffers(1, &(mesh.ebo)); }
	clear(mesh.textures);
}



void link_mesh_default(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode) {
	glBufferData(GL_ARRAY_BUFFER, points_count * sizeof(Point_N_UV), points, draw_mode);
	ADD_ATTRIB(0, Point_N_UV, pos);
	ADD_ATTRIB(1, Point_N_UV, uv);
	ADD_ATTRIB(2, Point_N_UV, n1);
	ADD_ATTRIB(3, Point_N_UV, n2);
	ADD_ATTRIB(4, Point_N_UV, n3);
}
void link_particles(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode) {
	glBufferData(GL_ARRAY_BUFFER, points_count * sizeof(Particle), points, draw_mode);
	ADD_ATTRIB(0, Particle, pos);
}
void link_mesh_uv(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode) {
	glBufferData(GL_ARRAY_BUFFER, points_count * sizeof(Point_UV), points, draw_mode);
	ADD_ATTRIB(0, Point_UV, pos);
	ADD_ATTRIB(1, Point_UV, uv);
}
void link_line_strip(void* points, u32 points_count, decltype(GL_STATIC_DRAW) draw_mode) {
	glBufferData(GL_ARRAY_BUFFER, points_count * sizeof(Strip_Node), points, GL_STATIC_DRAW);
	ADD_ATTRIB(0, Strip_Node, pos);
	ADD_ATTRIB(1, Strip_Node, norm);
	ADD_ATTRIB(2, Strip_Node, t);
}


void draw_default(const Mesh_Any& mesh, u32 uloc_diff, u32 uloc_spec, u32 uloc_norm) {
	for(const auto& tex : mesh.textures) {
		if(tex.info.type == aiTextureType_DIFFUSE) {
			set_uniform(uloc_diff, tex, 0);
		} else if(tex.info.type == aiTextureType_SPECULAR) {
			set_uniform(uloc_spec, tex, 1);
		} else if(tex.info.type == aiTextureType_HEIGHT) {
			set_uniform(uloc_norm, tex, 2); // TODO set default single pixel texture
		}
	}
	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void draw_uv(const Mesh_Any& mesh) {
	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void draw_particles(const Mesh_Any& cloud) {
	glBindVertexArray(cloud.vao);
	glDrawArrays(GL_POINTS, 0, cloud.points_count);
	glBindVertexArray(0);
}
void draw_line_strip(const Mesh_Any& strip) {
	glBindVertexArray(strip.vao);
	glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, strip.points_count);
	glBindVertexArray(0);
}
