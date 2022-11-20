#version 330 core

uniform float u_length;
uniform sampler2D u_texture;
uniform vec2 u_tex_size;
uniform float u_r;
uniform float u_time;

in struct Geom_Out {
	vec2 uv;
} geom_out;

out vec4 o_color;

void main() {
	vec2 uv = geom_out.uv;

	const float rad = 2, speed = 10;

	uv.x = mod(uv.x + u_time * speed, u_length / 3);
	float t = min(uv.x, u_length / 3 - uv.x) * rad;
	float b = exp(-t * t);

	vec3 color = vec3(b);

	o_color = vec4(color, 1);
}
