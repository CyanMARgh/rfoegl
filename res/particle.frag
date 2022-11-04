#version 330 core

uniform sampler2D u_tex;

in vec2 a_uv;
out vec4 a_color;  

void main() {
	a_color = 1 - texture(u_tex, a_uv * .5 + .5);

	// float brightness = pow(max(1 - length(a_uv), 0), 2) * 1.;
	// vec3 color = vec3(1, .3, .15);
	// a_color = vec4(color * brightness, 1);

	// if(ghost) {
	// 	gl_FragDepth = 0;
	// } else {
	// 	gl_FragDepth = gl_FragCoord.z;
	// }
}
