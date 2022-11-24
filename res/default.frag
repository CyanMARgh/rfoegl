#version 420 core

layout(binding = 0) uniform sampler2D u_tex_diff;
layout(binding = 1) uniform sampler2D u_tex_spec;
// uniform sampler2D u_tex;
uniform vec3 u_view_pos;

in struct Vert_Out {
	vec3 pos;
	vec3 norm;
	vec2 uv;
} vert_out;

out vec4 o_color; 

const vec3 light_pos = vec3(10, 10, 10);
const vec3 light_color = vec3(1., 1., 1.);

void main() {
	vec3 spec_color = texture(u_tex_spec, vec2(vert_out.uv.x, 1 - vert_out.uv.y)).xyz;
	vec3 diff_color = texture(u_tex_diff, vec2(vert_out.uv.x, 1 - vert_out.uv.y)).xyz;
	// vec3 obj_color = (1 + normalize(vert_out.norm)) / 2;

	// ambient
	float ambient_strength = .3;
	vec3 ambient = ambient_strength * light_color;

	// diffuse
	vec3 norm = normalize(vert_out.norm);
	vec3 light_dir = normalize(light_pos - vert_out.pos);
	vec3 diffuse = max(dot(norm, light_dir), 0) * light_color;

	// specular
	float specular_strength = 1;
	vec3 view_dir = normalize(u_view_pos - vert_out.pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	vec3 specular = pow(max(dot(view_dir, reflect_dir), 0), 16) * specular_strength * light_color;

	// specular2
	vec3 halfway_dir = normalize(light_dir + view_dir);
	vec3 specular2 = light_color * pow(max(dot(norm, halfway_dir), 0.0), 16);


	vec3 color = (ambient + diffuse) * diff_color + (gl_FragCoord.x < 600 ? specular : specular2) * spec_color;

	o_color = vec4(color, 1);
}
