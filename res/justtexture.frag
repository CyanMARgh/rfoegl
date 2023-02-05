#version 420 core

layout (binding = 0) uniform sampler2D u_tex_diff;
layout (binding = 1) uniform sampler2D u_tex_spec;
layout (binding = 2) uniform sampler2D u_tex_norm;


uniform vec3 u_view_pos;
uniform float u_time;

in struct Vert_Out {
	vec3 pos;
	vec2 uv;

	mat3 N;
} vert_out;

out vec4 o_color;

void main() {
	vec2 uv = vert_out.uv;
#ifdef FLIPUV
	uv.y = 1 - uv.y;
#endif
	o_color = texture(u_tex_diff, uv);
}