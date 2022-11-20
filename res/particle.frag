#version 330 core

uniform sampler2D u_tex;
uniform vec2 u_screen_size;
uniform float u_time;

in vec2 a_uv;
in float freq;
out vec4 color;  

float z_near = .02, z_far  = 130;

float linearize_depth(float depth)  {
	float z = depth * 2 - 1; 
	return (2 * z_near * z_far) / (z_far + z_near - z * (z_far - z_near));	
}

void main() {
	float d0 = linearize_depth(texture(u_tex, (gl_FragCoord.xy) / u_screen_size).r);
	float d1 = linearize_depth(gl_FragCoord.z);

	float depth_factor = clamp((d0 - d1) / 0.01, 0, 1);
	float dist_factor = pow(max(1 - length(a_uv), 0), 2) * 1.;
	float time_factor = sin((u_time + 10.) * (1.f + freq) * 2.) * .5 + .5;

	float brightness = depth_factor * dist_factor * time_factor;

	color = vec4(vec3(.3, .8, .1), brightness);
}
