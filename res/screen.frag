#version 330 core

uniform sampler2D u_tex0, u_tex1;

in vec2 uv;
out vec4 color;


float z_near = .02, z_far  = 100;
  
float linearize_depth(float depth)  {
    float z = depth * 2 - 1; 
    return (2 * z_near * z_far) / (z_far + z_near - z * (z_far - z_near));	
}

void main() {
	// color = vec4(vec3(linearize_depth(texture(u_tex, uv).r)), 1);
	color = uv.x > .5 ? texture(u_tex0, uv) : vec4(vec3(linearize_depth(texture(u_tex1, uv).r)), 1);
}