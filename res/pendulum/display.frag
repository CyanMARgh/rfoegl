#version 430 core

uniform sampler2D u_screen;

in vec2 uv;

out vec4 o_color;

const float PI = 3.1415926538;

vec3 twilight_shifted(float t) {
	const vec3 c0 = vec3(0.120488, 0.047735, 0.106111);
	const vec3 c1 = vec3(5.175161, 0.597944, 7.333840);
	const vec3 c2 = vec3(-47.426009, -0.862094, -49.143485);
	const vec3 c3 = vec3(197.225325, 47.538667, 194.773468);
	const vec3 c4 = vec3(-361.218441, -146.888121, -389.642741);
	const vec3 c5 = vec3(298.941929, 151.947507, 359.860766);
	const vec3 c6 = vec3(-92.697067, -52.312119, -123.143476);
	return c0 + t * (c1 + t * (c2 + t * (c3 + t * (c4 + t * (c5 + t * c6)))));
}
vec3 twilight_shifted_2(vec2 p) {
	return twilight_shifted(atan(p.x, p.y) / (PI * 2) + .5);
}

void main() {
	o_color = vec4(twilight_shifted_2(texture(u_screen, uv).xy), 0);
}