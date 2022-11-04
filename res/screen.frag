#version 330 core

uniform sampler2D u_tex;

in vec2 uv;
out vec4 color;

void main() {
	// float depth = 1 - texture(screen_tex, uv).x;

	// color = vec4(vec3(depth), 1);

	color = texture(u_tex, uv);
}