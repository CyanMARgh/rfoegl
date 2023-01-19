#version 430 core

uniform sampler2D u_screen;

in vec2 uv;
out vec4 o_color;

void main() {
	float br = texture(u_screen, uv).x;
	o_color = vec4(vec3(br), 1);
}