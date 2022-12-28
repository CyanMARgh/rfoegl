#version 430 core

layout (binding = 0) uniform sampler2D u_tex;
layout (binding = 1) uniform sampler2D u_depth;
uniform float u_time;

in vec2 uv;
out vec4 o_color;

float fPixelRepeat = 200.0;
float N = 6;
float M = 550;
float q = 1.5;

float z_near = .1, z_far  = 100;
float linearize_depth(float depth)  {
	float z = depth * 2 - 1; 
	return (2 * z_near * z_far) / (z_far + z_near - z * (z_far - z_near));	
}

float GetDepth(vec2 uv) {
	float d1 = texture(u_depth, uv).r;
	float d2 = linearize_depth(d1);
	float d3 = 10 - (d2 > 10 ? 10 : d2);
	return pow(d3, 1.6);

	// vec2 uvf = uv * 2.0 - 1.0;
	// uvf.x *= q;

	// float R = length(uvf);
	// return R >= .7 ? 0. : 4. * pow(.49 - R * R, .1);
}

vec4 Stereogram(vec2 P) {	
	for(int i = 0; i < 64; i++) {
		P.x += -1 / N  + GetDepth(P) / M;
		if(P.x < 0.0) break;
	}
	P.x = mod(P.x, 1 / N);
	return texture(u_tex, fract(P * N / vec2(1., q)));
}

void main() {	
	o_color = Stereogram(uv);
}
