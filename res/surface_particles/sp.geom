#version 430 core
layout (triangles) in;
layout (binding = 0) uniform sampler2D u_tex_diff;

#define MAX_VERTICES 80
layout (triangle_strip, max_vertices = MAX_VERTICES) out;

uniform mat4 u_trworld;
uniform mat4 u_trscreen;
uniform vec3 u_eye;

in struct Vert_Out {
	vec3 pos;
	vec2 uv;
} vert_out[];

out struct Geom_Out {
	vec2 uv;
	vec3 color;
	float t, r;
} geom_out;

const vec2 u_screen_scale = vec2(1.5, 1.);

float hash13(vec3 p3) {
    p3 = fract(p3 * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}
vec2 hash23(vec3 p3) {
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.xx + p3.yz) * p3.zy);
}
Vert_Out mix_(Vert_Out v1, Vert_Out v2, float t) {
	return Vert_Out(mix(v1.pos, v2.pos, t), mix(v1.uv, v2.uv, t));
}  

const float rad = .8;
void add_point(vec4 pos, vec2 delta) {
	geom_out.uv = delta;
	gl_Position = pos + vec4(delta * rad / u_screen_scale, 0, 0);
	EmitVertex();
}
void add_particle(vec3 p0, vec3 color, float r) {
	geom_out.color = color;
	vec4 p = u_trscreen * u_trworld * vec4(p0, 1);
	
	add_point(p, vec2(-1, -1) * r);
	add_point(p, vec2( 1, -1) * r);
	add_point(p, vec2(-1,  1) * r);
	add_point(p, vec2( 1,  1) * r);

	EndPrimitive();
}
void make_particle(Vert_Out v0, Vert_Out v1, Vert_Out v2, vec2 h, int id, float rfactor) {
	h = hash23(vec3(h, float(id)));
	geom_out.t = float(id);
	float r = (h.x * .5 + .5) * rfactor;
	geom_out.r = r;
	Vert_Out vi = mix_(mix_(v0, v1, h.x), v2, h.y * h.y);
#ifdef FLIPUV
	vi.uv.y = 1 - vi.uv.y;
#endif
	vec3 color = texture(u_tex_diff, vi.uv).rgb;
	add_particle(vi.pos, color, r + 0.2);
}
void main() {
	Vert_Out v0 = vert_out[0], v1 = vert_out[1], v2 = vert_out[2];
	vec2 h = hash23(vec3(hash13(v0.pos), hash13(v1.pos), hash13(v2.pos)));

	const float S0 = 1;
	vec3 N = cross(v1.pos - v0.pos, v2.pos - v0.pos);
	vec3 Nv = (u_trworld * vec4(N, 0)).xyz;
	vec3 eye_dir = u_eye - (u_trworld * vec4(v0.pos, 1)).xyz;
	float f = dot(normalize(Nv), normalize(eye_dir));
	if(f < 0) return;

	float S = length(N);
	int imax = min(MAX_VERTICES / 4, int(S / S0));

	for(int i = 0; i < imax; i++) {
		make_particle(v0, v1, v2, h, i, pow(f, .5));
	}
}



