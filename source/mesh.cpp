#include <cstdio>

#include "mesh.h"
#include "defer.h"

#define ADD_ATTRIB(id, struct_name, field_name)\
glVertexAttribPointer(id, sizeof(std::declval<struct_name>().field_name) / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(struct_name), (void*)offsetof(struct_name, field_name));\
glEnableVertexAttribArray(id);


void Mesh_Any::prepare(Mesh_Raw mesh_raw, const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode) {
	points_count = mesh_raw.points_count;
	indices_count = mesh_raw.indices_count;
	this->textures = textures;
	has_ebo = !!mesh_raw.indices;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	if(has_ebo) {
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(u32), mesh_raw.indices, draw_mode);
	}
}
Mesh_Any::Mesh_Any(
	std::function<void(Mesh_Raw, decltype(GL_STATIC_DRAW))> _attr_linker,
	std::function<void(const Mesh_Any&)> _painter,
	Mesh_Raw mesh_raw,
	const std::vector<Texture>& textures, decltype(GL_STATIC_DRAW) draw_mode
) : attr_linker(_attr_linker), painter(_painter), is_owner(true) {
	printf("+ mesh\n");
	prepare(mesh_raw, textures, draw_mode);
	attr_linker(mesh_raw, draw_mode);
	glBindVertexArray(0);
}
void Mesh_Any::clear() {
	printf("- mesh\n");
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	if(has_ebo) { glDeleteBuffers(1, &ebo); }
	// ::clear(textures); // TODO think about it
}
void Mesh_Any::move(Mesh_Any& other) {
	// printf("move\n");
	is_owner = other.is_owner;
	other.is_owner = false;	

	attr_linker = other.attr_linker; painter = other.painter;
	textures = std::move(other.textures); //TODO std::move maybe
	vbo = other.vbo; vao = other.vao; ebo = other.ebo;
	points_count = other.points_count, indices_count = other.indices_count;
	has_ebo = other.has_ebo;
}
Mesh_Any::Mesh_Any(Mesh_Any&& other) {
	// printf("move constructor\n");
	move(other);
}
Mesh_Any& Mesh_Any::operator=(Mesh_Any&& other) {
	// printf("move operator=\n");
	if(&other != this) {
		clear();
		move(other);
	}
	return *this;
}
Mesh_Any::~Mesh_Any() {
	// printf("mesh destructor: ");
	if(is_owner) {
		clear(); 
	}
}
void Mesh_Any::draw() {
	// printf("Mesh_Any::draw()->");
	painter(*this);
}


void link_mesh_default(Mesh_Raw mesh_raw, decltype(GL_STATIC_DRAW) draw_mode) {
	glBufferData(GL_ARRAY_BUFFER, mesh_raw.points_count * sizeof(Point_N_UV), mesh_raw.points, draw_mode);
	ADD_ATTRIB(0, Point_N_UV, pos);
	ADD_ATTRIB(1, Point_N_UV, uv);
	ADD_ATTRIB(2, Point_N_UV, n1);
	ADD_ATTRIB(3, Point_N_UV, n2);
	ADD_ATTRIB(4, Point_N_UV, n3);
}
void link_particles(Mesh_Raw mesh_raw, decltype(GL_STATIC_DRAW) draw_mode) {
	glBufferData(GL_ARRAY_BUFFER, mesh_raw.points_count * sizeof(Particle), mesh_raw.points, draw_mode);
	ADD_ATTRIB(0, Particle, pos);
}
void link_mesh_uv(Mesh_Raw mesh_raw, decltype(GL_STATIC_DRAW) draw_mode) {
	glBufferData(GL_ARRAY_BUFFER, mesh_raw.points_count * sizeof(Point_UV), mesh_raw.points, draw_mode);
	ADD_ATTRIB(0, Point_UV, pos);
	ADD_ATTRIB(1, Point_UV, uv);
}
void link_line_strip(Mesh_Raw mesh_raw, decltype(GL_STATIC_DRAW) draw_mode) {
	glBufferData(GL_ARRAY_BUFFER, mesh_raw.points_count * sizeof(Strip_Node), mesh_raw.points, draw_mode);
	ADD_ATTRIB(0, Strip_Node, pos);
	ADD_ATTRIB(1, Strip_Node, norm);
	ADD_ATTRIB(2, Strip_Node, t);
}

void draw_default(const Mesh_Any& mesh) {
	glBindVertexArray(mesh.vao);
	glDrawElements(GL_TRIANGLES, mesh.indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void draw_uv(const Mesh_Any& mesh) {
	// printf("draw uv (%d)\n", mesh.points_count);
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
