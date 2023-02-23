#version 330 core

uniform sampler2D u_color;

in vec2 uv;
out vec4 color;

void main() {
	vec4 cm = texture(u_color, uv);
	color = cm;
}