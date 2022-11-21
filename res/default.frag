#version 330 core

uniform sampler2D u_tex;
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
	vec3 obj_color = (1 + normalize(vert_out.norm)) / 2;

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
	vec3 specular = pow(max(dot(view_dir, reflect_dir), 0), 32) * specular_strength * light_color;

	vec3 color = (ambient + diffuse + specular) * obj_color;

	o_color = vec4(color, 1);
}
