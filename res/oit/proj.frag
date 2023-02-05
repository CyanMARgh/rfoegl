#version 420 core

layout (binding = 0) uniform sampler2D u_tex_diff;
layout (binding = 1) uniform sampler2D u_tex_spec;
layout (binding = 2) uniform sampler2D u_tex_norm;

layout (rgba32f, binding = 3) uniform image2D u_screen;

uniform vec3 u_view_pos;
uniform float u_time;

in struct Vert_Out {
	vec3 pos;
	vec2 uv;
	mat3 N;
} vert_out;

out vec4 o_color;

//TODO binding = 0 maybe (and somewhere else too)
layout (std140, binding = 0) uniform Shader_Data {
	vec3 colors[4];
};

void main() {
	vec2 uv = vert_out.uv;
#ifdef FLIPUV
	uv.y = 1 - uv.y;
#endif
	vec4 color = texture(u_tex_diff, uv);
	// color = mix(color, vec4(colors[int(gl_FragCoord.x > 600) + 2 * int(gl_FragCoord.y > 400)], 0), 1);
	color = vec4(colors[int(gl_FragCoord.x > 600) + 2 * int(gl_FragCoord.y > 400)].xyz, 1);
	// float br = (color.x + color.y + color.z) / 3;

	ivec2 coord = ivec2(gl_FragCoord.xy);

	// imageStore(u_screen, coord, color);
	// o_color = imageLoad(u_screen, coord);
	o_color = color;
}