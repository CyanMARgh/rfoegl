#version 330 core

uniform sampler2D u_tex;

in vec2 uv;
out vec4 color;  

void main() {
	// color = vec4(0, 0, 1, 1);
	color = (texture(u_tex, uv) + vec4(0, 0, 1, 1)) * .5;
	// color = vec4(uv, 1, 1);
}
