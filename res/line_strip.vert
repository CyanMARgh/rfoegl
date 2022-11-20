#version 330
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_norm;
layout (location = 2) in float a_time;

uniform vec3 u_zone_size;

out struct Vert_Out {
	vec3 norm;
	float time;
	int id;
} vert_out;

const vec3 u_screen_scale = vec3(1.5, 1., 1.);

void main() {
	gl_Position = vec4((a_pos / u_zone_size) * 2 - 1, 1);
	vert_out = Vert_Out(a_norm, a_time, gl_VertexID);
}