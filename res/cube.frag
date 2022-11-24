#version 330 core

uniform sampler2D u_tex;

in vec2 uv;
out vec4 color;  

void main() {
	// color = vec4(0, 0, 1, 1);
	// color = texture(u_tex, vec2(uv.x, 1 - uv.y));
	color = vec4(vec3(0), 1);
}
