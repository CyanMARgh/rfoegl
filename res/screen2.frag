#version 430 core

uniform sampler2D u_screen;

in vec2 uv;

out vec4 o_color;

void main() {
	o_color = texture(u_screen, uv);
}