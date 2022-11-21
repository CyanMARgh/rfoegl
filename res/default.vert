#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_norm;
layout (location = 2) in vec2 a_uv;

uniform mat4 u_trworld;
uniform mat4 u_trscreen;

out struct Vert_Out {
	vec3 pos;
	vec3 norm;
	vec2 uv;
} vert_out;

void main() {
	vec4 world_loc = u_trworld * vec4(a_pos, 1.);
	gl_Position = u_trscreen * world_loc;
	vert_out = Vert_Out(vec3(world_loc), a_norm, a_uv);
}