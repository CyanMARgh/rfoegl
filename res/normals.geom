#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 18) out;

uniform mat4 u_trscreen;

in struct Vert_Out {
	vec3 pos;
	vec2 uv;

	mat3 N;
} vert_out[];

out vec3 color;

void add_point(vec3 p) {
	gl_Position = u_trscreen * vec4(p, 1);
	EmitVertex();
}
const float r = .1;

void add_edge(vec3 n, vec3 col, int i) {
	color = col;
	add_point(vert_out[i].pos);
	add_point(vert_out[i].pos + n * r);
	EndPrimitive();
}
void add_3_edjes(int i) {
	add_edge(vert_out[i].N[0], vec3(1, 0, 0), i);
	add_edge(vert_out[i].N[1], vec3(0, 1, 0), i);
	add_edge(vert_out[i].N[2], vec3(0, 0, 1), i);
}

void main() {
	add_3_edjes(0);
	add_3_edjes(1);
	add_3_edjes(2);
}
