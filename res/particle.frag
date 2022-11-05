#version 330 core

uniform sampler2D u_tex;
uniform vec2 u_screen_size;

in vec2 a_uv;
out vec4 color;  

float z_near = .02, z_far  = 100;

float linearize_depth(float depth)  {
	float z = depth * 2 - 1; 
	return (2 * z_near * z_far) / (z_far + z_near - z * (z_far - z_near));	
}

void main() {
	float d0 = texture(u_tex, gl_FragCoord.xy / u_screen_size).r;
	float d1 = gl_FragCoord.z;
	float depth_factor = clamp((d0 - d1) / 0.01, 0, 1);
	float dist_factor = pow(max(1 - length(a_uv), 0), 2) * 1.;
	float brightness = depth_factor * dist_factor;

	color = vec4(vec3(1, .3, .15), brightness);
	// color = vec4(vec3(depth_factor), 1);

	// color = texture(u_tex, gl_FragCoord.xy * .5 + .5);

	//color = texture(u_tex, a_uv);

	// float depth = linearize_depth(texture(u_tex, gl_FragCoord.xy * .5 + .5).z);
	// color = vec4(vec3(depth), 1);


	// a_color = 1 - texture(u_tex, a_uv * .5 + .5);
	// color = vec4(1, 0, 0, 1);

	// bool is_hidden = gl_FragCoord.z < texture(u_tex, a_uv).r;

	// float brightness = is_hidden ? 0 : 1;//pow(max(1 - length(a_uv), 0), 2) * 1.;
	// color = vec4(vec3(1, .3, .15) * brightness, 1);

	// if(ghost) {
	// 	gl_FragDepth = 0;
	// } else {
	// 	gl_FragDepth = gl_FragCoord.z;
	// }
}
