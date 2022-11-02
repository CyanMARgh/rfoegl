#version 330 core

layout (location = 0) in vec3 position;
// layout (location = 1) in vec3 color;
// layout (location = 1) in vec2 _tex_uv;

uniform mat4 u_transform;
// uniform float u_time;

// out vec2 tex_uv;

void main() {
	gl_Position = u_transform * vec4(position, 1.);
	// tex_uv = _tex_uv;
}