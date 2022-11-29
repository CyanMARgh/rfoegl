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

struct Dir_Light {
	vec3 dir;
	vec3 amb, diff, spec;
};

struct Point_Light {
	vec3 pos;
	float C, L, Q;
	vec3 amb, diff, spec;
};

float shininess = 8;
const float pl_rad = 20;

vec2 flip(vec2 uv) {
	return vec2(uv.x, 1 - uv.y);
}
vec3 calc_dir_light(Dir_Light light, vec3 norm, vec3 view_dir) {
	// TODO Light, as type
	vec3 light_dir = normalize(-light.dir);
	float diff_c = max(dot(norm, light_dir), 0);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec_c = pow(max(dot(view_dir, reflect_dir), 0), shininess);

	vec3 amb = light.amb * texture(u_tex_diff, flip(vert_out.uv)).xyz;
	vec3 diff = light.diff * diff_c * texture(u_tex_diff, flip(vert_out.uv)).xyz;
	vec3 spec = light.spec * spec_c * texture(u_tex_spec, flip(vert_out.uv)).xyz;

	return amb + diff + spec;
}
vec3 calc_point_light(Point_Light light, vec3 norm, vec3 frag_pos, vec3 view_dir) {
	vec3 light_dir = normalize(light.pos - frag_pos);
	float diff_c = max(dot(norm, light_dir), 0);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec_c = pow(max(dot(view_dir, reflect_dir), 0), shininess);

	float dist = length(light.pos - frag_pos);
	float att = 1 / (light.C + light.L * dist + light.Q * dist * dist);

	vec3 amb = light.amb * texture(u_tex_diff, flip(vert_out.uv)).xyz;
	vec3 diff = light.diff * diff_c * texture(u_tex_diff, flip(vert_out.uv)).xyz;
	vec3 spec = light.spec * spec_c * texture(u_tex_spec, flip(vert_out.uv)).xyz;

	return (amb + diff + spec) * att;
}

out vec4 o_color; 

void main() {
	Point_Light light = Point_Light(
		vec3(sin(u_time * 2) * 2, 2, cos(u_time * 2) * 2),
		1, 4.5 / pl_rad, 75 / (pl_rad * pl_rad),
		vec3(.3), vec3(1), vec3(1)
	);

	vec3 n0 = normalize(texture(u_tex_norm, flip(vert_out.uv)).xyz * 2 - 1);
	vec3 norm = vert_out.N * n0;//(gl_FragCoord.x < 600 ? n0 : vec3(0, 0, 1));

	vec3 color = calc_point_light(light, norm, vert_out.pos, normalize(u_view_pos - vert_out.pos));

	o_color = vec4(pow(color, vec3(1 / 2.2)), 1);

	// o_color = vec4((normalize(norm) + 1) / 2, 1);
	// float ___ = abs(dot(vert_out.N[0], vert_out.N[1])) + abs(dot(vert_out.N[1], vert_out.N[2])) + abs(dot(vert_out.N[2], vert_out.N[0])); 
	// o_color = vec4(vec3(___), 1);
	// o_color = vec4(.3 * (normalize(vert_out.N[0]) + 1) / 2, 1);
	// o_color = vec4(flip(vert_out.uv), 0, 1);

	// o_color = vec4(vert_out.uv, 0, 1);

	// o_color = vec4(normalize(norm) * .5 + .5, 1);

	// o_color = vec4(norm * .5 + .5, 1);
}
