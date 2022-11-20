#define GLEW_STATIC
#include <GL/glew.h>
#include <cstdio>

#include "mesh.h"

Mesh_UV::Mesh_UV(Point_UV* points, u32 points_count, u32* indices, u32 indices_count) {
	// printf("(+)\n");
	points_size = points_count * sizeof(Point_UV);
	this->indices_count = indices_count;
		// indices_size = indices_count * sizeof(u32);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points_size, points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point_UV), (void*)offsetof(Point_UV, pos));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Point_UV), (void*)offsetof(Point_UV, uv));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(u32), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
}
Mesh_UV::Mesh_UV(Mesh_UV&& other) {
	// printf("move(constructor)\n");
	VAO = other.VAO, VBO = other.VBO, EBO = other.EBO;
	indices_count = other.indices_count, points_size = other.points_size;
	other.active = false;
}
Mesh_UV& Mesh_UV::operator= (Mesh_UV&& other) {
	// printf("move(operator=)\n");
	if(this == &other) return *this;
	VAO = other.VAO, VBO = other.VBO, EBO = other.EBO;
	indices_count = other.indices_count, points_size = other.points_size;
	other.active = false;
	return *this;
}

Mesh_UV::~Mesh_UV() {
	if(active) {
		// printf("(-)\n");
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);		
	} else {
		// printf("deleteing empty\n");
	}
}

Particle_Cloud::Particle_Cloud(Particle* particles, u32 particles_count) {
	this->particles_count = particles_count;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, particles_count * sizeof(Particle), particles, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, pos));
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
Particle_Cloud::~Particle_Cloud() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);	
}

Line_Strip::Line_Strip(Strip_Node* points, u32 count) {
	this->points_count = count;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points_count * sizeof(Strip_Node), points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Strip_Node), (void*)offsetof(Strip_Node, pos));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Strip_Node), (void*)offsetof(Strip_Node, norm));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Strip_Node), (void*)offsetof(Strip_Node, t));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}
Line_Strip::~Line_Strip() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


void draw(const Mesh_UV& mesh_UV) {
	glBindVertexArray(mesh_UV.VAO);
	glDrawElements(GL_TRIANGLES, mesh_UV.indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void draw(const Particle_Cloud& particle_cloud) {
	glBindVertexArray(particle_cloud.VAO);
	glDrawArrays(GL_POINTS, 0, particle_cloud.particles_count);
	// glDrawArraysInstanced(GL_POINTS, 0, particle_cloud.particles_count);
	glBindVertexArray(0);
}
void draw(const Line_Strip& line_strip) {
	glBindVertexArray(line_strip.VAO);
	glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, line_strip.points_count);
	// glDrawArraysInstanced(GL_POINTS, 0, particle_cloud.particles_count);
	glBindVertexArray(0);
}
