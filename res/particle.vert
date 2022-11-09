#version 330 core

layout (location = 0) in vec3 position;
// layout (location = 1) in vec3 color;
// layout (location = 1) in vec2 _tex_uv;

uniform mat4 u_transform;
out float _freq;

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

void main() {
	gl_Position = u_transform * vec4(position, 1.);
	_freq = rand(vec2(rand(position.xy), position.z));
}