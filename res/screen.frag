#version 330 core

uniform sampler2D u_tex;

in vec2 uv;
out vec4 color;

void main() {
	color = texture(u_tex, uv);
}