#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

const vec2 u_screen_size = vec2(1200, 800);
uniform float u_particle_size = 30;

in float _freq[];

out vec2 a_uv;
out float freq;

void add_point(vec4 p0, vec2 dxy, vec4 M) {
	gl_Position = p0 + vec4(dxy, 0, 0) * M; 
	a_uv = dxy;
	EmitVertex();
}

void main() {
	vec4 M = vec4(u_particle_size / u_screen_size, 1., 1.), p0 = gl_in[0].gl_Position;
#ifdef SCREEN_SCALE
	p0.xyz /= p0.w; p0.w = 1;
#else
	M.xyz *= 4;
#endif

	freq = _freq[0];

	add_point(p0, vec2(-1, -1), M);
	add_point(p0, vec2( 1, -1), M);
	add_point(p0, vec2(-1,  1), M);
	add_point(p0, vec2( 1,  1), M);

	EndPrimitive();
}