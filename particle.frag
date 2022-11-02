#version 330 core

in vec2 a_uv;
out vec4 a_color;  

// uniform bool ghost;

void main() {
	float brightness = pow(max(1 - length(a_uv), 0), 2) * 1.;
	vec3 color = vec3(1, .3, .15);
	a_color = vec4(color, brightness);

	// if(ghost) {
	// 	gl_FragDepth = 0;
	// } else {
	// 	gl_FragDepth = gl_FragCoord.z;
	// }
}
