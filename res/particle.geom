#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// uniform vec2 u_screen_size;

const vec2 u_screen_size = vec2(800, 600);
const float particle_size = 30;

in float _freq[];

out vec2 a_uv;
out float freq;

void main() {
	vec4 M = vec4(particle_size / u_screen_size, 1., 1.), p0 = gl_in[0].gl_Position;

	freq = _freq[0];

	gl_Position = p0 + vec4(-1, -1, 0, 0) * M; 
	a_uv = vec2(-1, -1);
	EmitVertex();
	gl_Position = p0 + vec4( 1, -1, 0, 0) * M; 
	a_uv = vec2(1, -1);
	EmitVertex();
	gl_Position = p0 + vec4(-1,  1, 0, 0) * M; 
	a_uv = vec2(-1, 1);
	EmitVertex();
	gl_Position = p0 + vec4( 1,  1, 0, 0) * M; 
	a_uv = vec2(1, 1);
	EmitVertex();
	EndPrimitive();
}


// #version 330 core
// layout (points) in;
// layout (triangle_strip, max_vertices = 4) out;

// // uniform vec2 u_screen_size;

// const vec2 u_screen_size = vec2(800, 600);
// const float particle_size = 30;

// in float _freq[];

// out vec2 a_uv;
// out float freq;

// void main() {
// 	vec4 M = vec4(particle_size / u_screen_size, 1., 1.), p0 = gl_in[0].gl_Position;

// 	freq = _freq[0];

// 	gl_Position = p0 + vec4(-1, -1, 0, 0) * M; 
// 	a_uv = vec2(-1, -1);
// 	EmitVertex();
// 	gl_Position = p0 + vec4( 1, -1, 0, 0) * M; 
// 	a_uv = vec2(1, -1);
// 	EmitVertex();
// 	gl_Position = p0 + vec4(-1,  1, 0, 0) * M; 
// 	a_uv = vec2(-1, 1);
// 	EmitVertex();
// 	gl_Position = p0 + vec4( 1,  1, 0, 0) * M; 
// 	a_uv = vec2(1, 1);
// 	EmitVertex();
// 	EndPrimitive();
// }

