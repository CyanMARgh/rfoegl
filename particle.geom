#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// uniform vec2 u_screen_size;

const vec2 u_screen_size = vec2(800, 600);
const float particle_size = 70;

out vec2 a_uv;

void main() {
	vec4 M = vec4(particle_size / u_screen_size, 1., 1.);

	gl_Position = gl_in[0].gl_Position + vec4(-1, -1, 0, 0) * M; 
	a_uv = vec2(-1, -1);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(-1,  1, 0, 0) * M; 
	a_uv = vec2(-1, 1);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4( 1, -1, 0, 0) * M; 
	a_uv = vec2(1, -1);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4( 1,  1, 0, 0) * M; 
	a_uv = vec2(1, 1);
	EmitVertex();
	EndPrimitive();
}  