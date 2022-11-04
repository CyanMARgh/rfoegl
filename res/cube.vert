#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 a_uv;

uniform mat4 u_transform;

out vec2 uv;

void main() {
	gl_Position = u_transform * vec4(position, 1.);
	uv = a_uv;
}