#version 430 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;

layout (location = 2) in vec3 a_n1;
layout (location = 3) in vec3 a_n2;
layout (location = 4) in vec3 a_n3;

out struct Vert_Out {
	vec3 pos;
	vec2 uv;
} vert_out;

void main() {
	vert_out = Vert_Out(a_pos, a_uv);
}