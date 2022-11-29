#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;

layout (location = 2) in vec3 a_n1;
layout (location = 3) in vec3 a_n2;
layout (location = 4) in vec3 a_n3;

uniform mat4 u_trworld;
uniform mat4 u_trscreen;

out struct Vert_Out {
	vec3 pos;
	vec2 uv;

	mat3 N;
} vert_out;

void main() {
	vec4 world_loc = u_trworld * vec4(a_pos, 1.);
	gl_Position = u_trscreen * world_loc;

	vec3 n1 = (u_trworld * vec4(a_n1, 0)).xyz; // N
	vec3 n2 = (u_trworld * vec4(a_n2, 0)).xyz; // T
	// vec3 n3 = (u_trworld * vec4(a_n3, 0)).xyz; // B

	n2 = normalize(n2 - dot(n2, n1) * n1);

	vec3 n3 = cross(n1, n2);

	vert_out = Vert_Out(vec3(world_loc), a_uv, mat3(a_n2, a_n3, a_n1));
}